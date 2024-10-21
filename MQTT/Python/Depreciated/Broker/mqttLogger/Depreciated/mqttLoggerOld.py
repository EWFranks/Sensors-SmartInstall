"""

Logs all incoming traffic from MQTT broker to a file for prototype Knowledge Core testing
Author: Sterling Fisher

"""

import paho.mqtt.client as mqtt
import time
import datetime
import configparser
import json
import os.path
import requests

file = None
boardDict = {}

def readConfig():
    """ Reads the config.ini file and returns a dictionary of the values. """

    filePath = os.path.join( os.path.dirname(__file__), "config.ini" )

    config = configparser.ConfigParser()
    config.read(filePath)


    # FORMAT: dict['brokerIP'] = config['MQTT']['brokerIP']
    dict = {}

    for section in config.sections():                   # For each section
        for option in config.options(section):          # For each option in that section
            dict[option] = config[section][option]      # Add it to the dictionary under that option

    return dict

def get_board_data():
    """ Reads the board data file and returns a dictionary of the values. """

    filePath = os.path.join( os.path.dirname(__file__), "Data/board_zoning.json" )

    with open(filePath) as json_file:
        data = json.load(json_file)

    return data["Boards"]


def new_file():
    """ Creates a new file for the current date and returns the file object. """

    filename = "mqtt log " + datetime.date.today().strftime("%m_%d_%Y") + ".txt"
    print(filename)
    return open(filename, 'w')


def generate_post(messageText, topic, timeReceived):
    """ Generates a dictionary of the POST request data. """

    dict = {}

    messageParts = messageText.split(' ')   # Split the message into reading and unit | 25.0 Celsius -> [25.0, Celsius]
    topicParts = topic.split('/') # Split the topic into Device ID and Reading Type | A0A0/temperature -> [A0A0, temperature]\
    # boardParts = get_board_data()

    reading = {}
    reading["reading_type"] = topicParts[0]

    if reading["reading_type"] == "motion":
        reading["reading_type"] = "pir_motion"

    reading["reading_unit"] = messageParts[1]

    if reading["reading_type"] == "pir_motion":
        reading["reading_unit"] = "flag"

    reading["reading_val_type"] = "number"

    if reading["reading_type"] == "pir_motion":
        reading["reading_val_type"] = "boolean"

    if(reading["reading_type"] == "pir_motion"):
        if messageParts[0] == "Motion":
            reading["reading_val"] = True
        else:
            reading["reading_val"] = False
        
    else:
        reading["reading_val"] = float(messageParts[0])
    
    reading["reading_time"] = timeReceived
    reading["hardware_info"] = {}

    reading["hardware_info"]["sensor_id"] = topicParts[1]
    reading["hardware_info"]["sensor_device"] = "Arduino MKR 1010"



    dict['installation_id'] = "rayzor"
    dict["collector_id"] = "rayzor2015"
    dict["device_reports"] = {}

    dict["device_reports"]["Zone-2015"] = {}

    dict["device_reports"]["Zone-2015"]["readings"] = []
    dict["device_reports"]["Zone-2015"]["readings"].append(reading)




    
    # dict['TimeReceived'] = timeReceived
    
    # dict["Reading"] = messageParts[0]
    # if len(messageParts) > 1:
    #     dict["Unit"] = messageParts[1]
 
    # dict["ReadingType"] = topicParts[0]

    # dict["BoardInfo"] = {}
    # dict["BoardInfo"]["DeviceID"] = topicParts[1]

    # if topicParts[1] in boardDict:
    #     dict["BoardInfo"]["Building"] = boardDict[topicParts[1]]["Building"]
    #     dict["BoardInfo"]["Room"] = boardDict[topicParts[1]]["Room"]
    #     dict["BoardInfo"]["Zone"] = boardDict[topicParts[1]]["Zone"]


    print(dict)
    return dict

def send_post(dict):
    """ Sends a POST request to the Knowledge Core API. """

    try:

        #time.sleep(.25)

        headers = {"Content-Type" : "application/json"}

        postRequest = requests.post("http://10.0.2.154:8000/api/data", json=dict, headers=headers, timeout=5)
        #postRequest = requests.post("http://10.0.2.108:8000/api/data", json=dict, headers=headers, timeout=5)
        #postRequest = requests.post("http://192.168.2.238:5000/api/data", json=dict, headers=headers, timeout=5)
        return postRequest.text
    except requests.exceptions.RequestException:
        print("Error: Could not send POST request to API.")
        return 0



# Thanks to http://www.steves-internet-guide.com/into-mqtt-python-client/
def on_message(client, userdata, message):
    """ Callback method for when a message is received. """

    print("message received = ", str(message.payload.decode("utf-8")))
    print("message topic = ", message.topic)
    #print("message qos=", message.qos)
    #print("message retain flag = ", message.retain)

    messageText = str(message.payload.decode("utf-8"))
    topic = message.topic
    #timeReceived = datetime.datetime.now().strftime("%H:%M:%S")
    timeReceived = datetime.datetime.now().astimezone().strftime("%Y-%m-%dT%H:%M:%S.%f%z")

#TODO: Generate POST request
    post_dict = generate_post(messageText, topic, timeReceived)

    print( send_post(post_dict) ) # Send to API, print response

# Write to file (immediately)
    #print("name: " + file.name)
    #file.write("[" + timeReceived + "] " + topic + " : " + messageText + "\n") 
    file.write(str( post_dict ) + "\n")
    file.flush() # Write to file immediately
    #print("Next message:")


if __name__ == '__main__':

# Read Config File
    configDict = readConfig()
    print(configDict)

# Get Board Data
    boardDict = get_board_data()

    # client = mqtt.Client("142")
    # client.connect("10.42.42.10")
    # client.subscribe("#")

# Connect to MQTT Broker
    client = mqtt.Client()   # Create new instance of client without ID
    client.connect( configDict['broker'], int(configDict['port']) ) # Connect to broker
    client.subscribe( configDict['topic'] ) # Subscribe to topic

    client.on_message = on_message # attach callback method


# Code to handle new file for each day
    while True: # Run forever

        file = new_file() # Create a new file for current date
        print("name: " + file.name)

        timeInterval = time.time()
        #client.loop_start() # Start receiving messages
        
        while( time.time() - timeInterval < 86400 ): # Continue for 24 hours
            #print(client.is_connected())
            #time.sleep(1)
            client.loop()
        #time.sleep(86400) # Continue for 24 hours
        
        #client.loop_stop()

        file.close() # Close the file, loop to create new file for next day

        

