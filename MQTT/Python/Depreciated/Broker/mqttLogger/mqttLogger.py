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

import queue

file = None
boardDict = {} 
configDict = {}

messageQueue = queue.Queue() # Queue to hold received messages to be processed

timeLastMessage = -1 # Times the interval between on_message() method calls 

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

    filename = "mqtt_log_" + datetime.date.today().strftime("%m_%d_%Y") + ".txt"
    filePath = os.path.join(os.path.dirname(__file__), "logs/" + filename)
    
    print(filename)
    return open(filePath, 'w')



def determineType(reading_val):
    """ Determines the type of the reading value. """

    try:
        float(reading_val)
        return float(reading_val), "number"
    except:
        pass

    if reading_val == "true" or reading_val == "false":
        return bool(reading_val), "boolean"

    return reading_val, "string"




def generate_post(messageText, topic, timeReceived):
    """ Generates a dictionary of the POST request data. """

    print(topic)
    print(messageText)

    zone = "Unknown"

    try:

        deviceID = topic.split("/")[1]

        if deviceID in boardDict and "Zone" in boardDict[deviceID]:
            zone = boardDict[deviceID]["Zone"]

        readingsDict = {
            'device_reports' : {
                zone : json.loads(messageText)
            }
        }

        for reading in readingsDict['device_reports'][zone]["readings"]:
            reading["reading_time"] = timeReceived
            reading["reading_val"], reading["reading_val_type"] = determineType( reading["reading_val"] )

            reading["hardware_info"] = {}
            reading["hardware_info"]["sensor_id"] = deviceID

    except Exception as e:
        print("Error: Could not parse message.\n")
        print(e)
        return 0


    print(readingsDict)
    return readingsDict



def send_post(dict):
    """ Sends a POST request to the Knowledge Core API. """

    try:

        headers = {"Content-Type" : "application/json"}
        postRequest = requests.post(f"http://{configDict['kc']}:8000/api/data", json=dict, headers=headers, timeout=5)

        #postRequest = requests.post("http://192.168.2.238:5000/api/data", json=dict, headers=headers, timeout=5)

        return postRequest.text
    except requests.exceptions.RequestException:
        print("Error: Could not send POST request to API.")
        return 0
    
prevMessages = {}


    
def process_messages():
    """ Dequeues and processes the received messages in the queue. """

    if messageQueue.empty():
        return
    
    messageText, topic, timeReceived = messageQueue.get()

    while messageQueue.qsize() >= 3: # Just keep getting the next message until there are only 3 left
        messageText, topic, timeReceived = messageQueue.get()


# The code for making sure messages sent were unique. This should not be an issue anymore.
    # try:
    #     if messageText == prevMessages[topic]:
    #         return
    # except:
    #     pass
        
    prevMessages[topic] = messageText


    #Generate POST request to KC API
    post_dict = generate_post(messageText, topic, timeReceived)

    print( send_post(post_dict) ) # Send to API, print response

# Write to file (immediately)
    file.write(str( post_dict ) + "\n")
    file.flush() # Write to file immediately



# Thanks to http://www.steves-internet-guide.com/into-mqtt-python-client/
def on_message(client, userdata, message):
    """ Callback method for when a message is received. """

    global timeLastMessage
    timeLastMessage = time.time()

    #print("message received = ", str(message.payload.decode("utf-8")))
    #print("message topic = ", message.topic)
    #print("message qos=", message.qos)
    #print("message retain flag = ", message.retain)

    messageText = str(message.payload.decode("utf-8"))
    topic = message.topic
    #timeReceived = datetime.datetime.now().strftime("%H:%M:%S")
    timeReceived = datetime.datetime.now().astimezone().strftime("%Y-%m-%dT%H:%M:%S.%f%z")

    messageQueue.put( (messageText, topic, timeReceived) )


def check_connection():
    """ Checks the connection to the MQTT broker and reconnects if necessary. """

    global timeLastMessage
    if time.time() - timeLastMessage > 10: # If it's been 10 seconds since the last message

        # Disconnect
        client.loop_stop() # Stop receiving messages
        client.disconnect() # Disconnect from broker

        # Reconnect
        client.connect( configDict['broker'], int(configDict['port']) ) # Connect to broker
        client.subscribe( configDict['topic'] ) # Subscribe to topic
        client.on_message = on_message # attach callback method

        timeLastMessage = time.time()
        client.loop_start() # Start receiving messages


if __name__ == '__main__':

    timeLastMessage = time.time()

# Read Config File
    configDict = readConfig()
    print(configDict)

# Get Board Data
    boardDict = get_board_data()


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

        #threading.Thread(target=process_messages_threaded).start() # If we want to process messages in a separate thread

        client.loop_start() # Start receiving messages
        
        while( time.time() - timeInterval < 86400 ): # Continue for 24 hours
            check_connection()
            process_messages()
        
        client.loop_stop()

        file.close() # Close the file, loop to create new file for next day

        

