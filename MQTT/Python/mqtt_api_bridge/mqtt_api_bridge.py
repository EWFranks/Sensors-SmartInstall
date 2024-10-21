""" A simple, customizable MQTT client that translates GET API requests into MQTT readings, and translates MQTT commands into API requests"""

import requests
import paho.mqtt.client as mqtt
import configparser

import os

import json
import time



client : mqtt.Client # The MQTT client

configDict : dict # Holds the config info from the INI file
deviceInfo : dict # Holds the device info from the JSON file

deviceHistory: dict = {} # For each device, holds the time since the last interval, the time since the last message, and the last message sent



# ---------- Command Handler ---------- #


def on_message(client, userdata, message):
    """ Callback for when a command message is received through MQTT. """

    global deviceInfo

    topic = message.topic
    messageBody = message.payload.decode("utf-8")

    print( f"Received message from {topic}: {messageBody}" )

    topic = topic.split("/")

    deviceID = topic[1]
    command = topic[2] # Initialize the command with the first part of the topic, additional subtopics will be appended

    # Append command with the rest of the topic
    for idx in range(3, len(topic)):
        command += "/" + topic[idx]


    print(f"Command: {command}")
    if command in deviceInfo[deviceID]["commands"]:
        send_command(deviceID, command, messageBody) # Message body is the arguments



def send_command(deviceID, command, args):
    """ Sends a command to the device through the API. """

    global deviceInfo
    command_info = deviceInfo[deviceID]["commands"][command]

    # If the command is a GET request, send the response to the MQTT broker
    if command_info["request_type"] == "GET":
        responseString = json.dumps( get_response(command_info["request_url"], deviceInfo[deviceID]["headers"]) )
        client.publish(f"data/{deviceID}/state", responseString)
    
    # If the command is a PUT request, send the request to the API
    elif command_info["request_type"] == "PUT":
        json_data = mqtt_args_json(args, command_info["request_data"])
        put_response(command_info["request_url"], deviceInfo[deviceID]["headers"], json_data)

    # TODO: Add POST request support, if needed



def mqtt_args_json(mqtt_args : str, json_template : dict):
    """ Converts the MQTT arguments into the given JSON dict template. """

    request_json = json_template # Create a copy of the sample json to use for the request

    if mqtt_args not in ["",  " ",  ","]: # If there are no arguments, just send the data
        args = mqtt_args.split(",")

        # Bad practice, but it works: pop the first argument and assign it to the first datapoint, etc.
        # Issue (kinda): dictionary isn't supposed to be ordered by definition, but iterates in sequence
        for datapoint in request_json:
            if len(args) > 0: # If there are still arguments left and the argument isn't empty
                request_json[datapoint] = detect_type(args.pop(0))

    return request_json



def detect_type(string):
    """ Detects the type of the string and returns the value. """

    if string == "True": return True
    elif string == "False": return False
    
    else:
        try:    
            return float(string)
        except ValueError:
            pass

    return string




# ---------- Reading Handler ---------- #


def get_readings():
    """ Gets the readings from the API, specified by the JSON, and sends them to the MQTT broker. """

    global deviceInfo, deviceHistory

    for device_id in deviceInfo: # For each device (loops through the keys of the dictionary)
        if reading_interval(device_id): # If it's time to get the readings
            readingsJson = {"readings" : []}

            for requestInfo in deviceInfo[device_id]["readings"]["requests"]: # For each http_request that gets readings (loops through the objects of the array)
                
                # Get the response JSON from the API
                response = get_response(requestInfo["request_url"], deviceInfo[device_id]["headers"]) 

                if response == None: # Skip to next request if there was an error getting the response
                    continue 


                # All readings are obtained from one or more requests, so we need to get each readings from the current request
                for readingInfo in requestInfo["request_readings"]: 
                    curValue = valFromJson(response, readingInfo["request_path"]) # Get the reading value from the response JSON by following the path

                    # Add the reading to the readings JSON
                    reading_object = build_reading_object(readingInfo["reading_type"], readingInfo["reading_unit"], curValue)
                    readingsJson["readings"].append(reading_object)

            # Sending the readings:
            jsonString = json.dumps(readingsJson) # Convert the readings into a JSON string for sending

            # DEBUG: print the readings
            if len(readingsJson["readings"]) > 0:
                print("\n" + jsonString)

                if check_repeat(device_id, jsonString): # If the message is not a repeat message, send it
                    continue # Stop, go to next Device ID
                

                # Send the readings to the MQTT broker
                client.publish(f"readings/{device_id}", json.dumps(readingsJson)) 
                print("Sent at time " + str(time.time() - float(deviceHistory[device_id]["timeSinceLastMessage"]))) # DEBUG: print the time since the last message

                # Set history for the device for next interval
                deviceHistory[device_id]["timeSinceLastMessage"] = time.time()
                deviceHistory[device_id]["lastMessage"] = jsonString



def valFromJson(jsonDict, path : list):
    """ Returns the value from the JSON dict at the given path. """

    valPath = jsonDict # Start at the root of the response

    if len(path) != 0: # If the response is a JSON
        for requestStep in path: # For each step in the designated path
            valPath = valPath[requestStep] # set your current "position" to the value at that step
    
    return valPath # The value at the end of the path is the value we want



def build_reading_object(reading_type, reading_unit, reading_val):
    """ Builds a reading object to add to the MQTT JSON payload from the given parameters. """

    return {
        "reading_type" : reading_type,
        "reading_unit" : reading_unit,
        "reading_val" : reading_val
    }



def check_repeat(device_id, jsonString):
    """ Checks if a message is a repeat message, and if a repeat message needs to be sent.
        Returns True if the message is a repeat message, False otherwise. """
    
    global deviceInfo, deviceHistory

    # If we do not allow normal repeats and it is not time to send repeats for heartbeat, return whether the message is the same as the last one
    if not deviceInfo[device_id]["readings"]["allow_normal_repeats"] and not heartbeat_interval(device_id): 
        return deviceHistory[device_id]["lastMessage"] == jsonString # Return whether the message is the same as the last one

    return False # Send the message



def reading_interval(device_id):
    """ Checks if it's time to get the readings from a given device.
    Returns true if the device's 'reading_interval' time has passed since last interval, false otherwise. """

    global deviceInfo, deviceHistory

    readingIntervalSeconds = deviceInfo[device_id]["readings"]["reading_interval"]/1000. # Convert to seconds

    # If the time since the last interval is greater than or equal to the device's interval time    
    readingReady = time.time() - float(deviceHistory[device_id]["timeSinceInterval"]) >= readingIntervalSeconds 

    # If it's time to get the readings, reset the interval timer
    if readingReady:
        deviceHistory[device_id]["timeSinceInterval"] = time.time()
    
    return readingReady



def heartbeat_interval(device_id):
    """ If the device's 'allow_repeats' is false, checks whether it is time to send the repeats anyways, to verify the last readings. """
    
    global deviceInfo, deviceHistory

    heartbeatReady = time.time() - float(deviceHistory[device_id]["timeSinceLastMessage"]) >= deviceInfo[device_id]["readings"]["repeat_heartbeat"]/1000.
    return heartbeatReady






def init_device_history():
    """ Initializes the device history. """

    global deviceInfo, deviceHistory

    for device_id in deviceInfo:
        deviceHistory[device_id] = {
            "timeSinceInterval": 0,
            "timeSinceLastMessage" : 0,
            "lastMessage" : None
        }




 # ---------- API Handler ---------- #


def get_response(url, headers):
    """ Simple method to send a GET request to the API. """

    try:

        response = requests.get(url, headers=headers) # Send the request
        
    except requests.exceptions.ConnectionError:
        print("Connection Error")
        return None
    except requests.exceptions.Timeout:
        print("Timeout Error")
        return None
    except requests.exceptions.RequestException as e:
        print("Unknown Error:")
        print(e)
        return None

    # Ensure the response was successful
    if response.status_code != 200:
        print("Error: " + str(response.status_code))
        return None

    print(response.json())
    return response.json()

    

def put_response(url, headers, json_data):
    """ Simple method that parses the arguments from mqtt . """

    try:

        response = requests.put(url, headers=headers, json=json_data) # Send the request

    except requests.exceptions.ConnectionError:
        print("Connection Error")
        return None
    except requests.exceptions.Timeout:
        print("Timeout Error")
        return None
    except requests.exceptions.RequestException as e:
        print("Unknown Error:")
        print(e)
        return None
    
    # Ensure the response was successful
    if response.status_code != 200:
        print("Error: " + str(response.status_code))
        return
    
    print(response.json())




# ---------- General/Setup Functions ---------- #


def connect_mqtt():
    """ Connects to the MQTT broker. """

    global client, configDict

    client = mqtt.Client()
    client.username_pw_set(configDict['mqtt_user'], configDict['mqtt_pass'])
    # client.username_pw_set("datamanager", "MaOvJr9ZkICOqsacFeQdJHtc")

    client.connect( configDict['broker'], int(configDict['port']) )
    
    # time.sleep(3)
    #print("Connected?: ", client.is_connected())



def subscribe_commands():
    """ Subscribes to the commands topic. """

    global client, configDict

    client.on_message = on_message

    for device in deviceInfo:
        client.subscribe( f"commands/{device}/#" )
        print( f"Subscribed to commands/{device}/#" )



def readConfig():
    """ Reads the config file and returns a dictionary of the config. """

    config = configparser.ConfigParser()

    configPath = os.path.dirname(os.path.realpath(__file__)) + '/instance/config.ini'
    config.read(configPath)


    # FORMAT: dict['brokerIP'] = config['MQTT']['brokerIP']
    dict = {}

    for section in config.sections():                   # For each section
        for option in config.options(section):          # For each option in that section
            dict[option] = config[section][option]      # Add it to the dictionary under that option

    return dict



def readDeviceInfo():
    """ Reads the device info from the Instance JSON file. """

    global deviceInfo
    jsonPath = os.path.dirname(os.path.realpath(__file__)) + '/instance/device_info.json'

    with open(jsonPath, 'r') as jsonFile:
        deviceInfo = json.loads(jsonFile.read())





def main():
    """ Main function. """

    global configDict
    global deviceInfo

    configDict = readConfig()

    readDeviceInfo()
    init_device_history()

    connect_mqtt()
    subscribe_commands()

    #client.loop_forever()
    client.loop_start()

    print("Commands Handled. Readings!")

    while True:
        get_readings()
        time.sleep(0.1)

    client.loop_stop()

if __name__ == "__main__":
    main()
