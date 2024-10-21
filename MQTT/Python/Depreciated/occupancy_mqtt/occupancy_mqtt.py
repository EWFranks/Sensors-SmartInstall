import requests
import configparser
import time
import paho.mqtt.client as mqtt
import os

import json


client : mqtt.Client
configDict : dict
timeLastMessage : float

def get_occupancy():
    """ Gets the occupancy from the SafeSpace API. """
    getRequest = requests.get("https://app.safespace.io/api/raw-data/live-occupancy/2a9b3336")
    return getRequest.text



def readConfig():
    config = configparser.ConfigParser()

    configPath = os.path.dirname(os.path.realpath(__file__)) + '/instance/config.ini'
    config.read(configPath)


    # FORMAT: dict['brokerIP'] = config['MQTT']['brokerIP']
    dict = {}

    for section in config.sections():                   # For each section
        for option in config.options(section):          # For each option in that section
            dict[option] = config[section][option]      # Add it to the dictionary under that option

    return dict

def send_occupancy(occupancy):
    """ Sends the occupancy to the MQTT broker as a JSON string. """

    global timeLastMessage, client, configDict
    
    timeLastMessage = time.time()


    occupancyDict = {
        "readings" : [
            {
                "reading_type" : "occupancy",
                "reading_unit" : "people",
                "reading_val" : occupancy
            }
        ]
    }

    topic = "readings/" + configDict["device_id"]
    print(json.dumps(occupancyDict))
    client.publish(topic, json.dumps(occupancyDict))



def connect_mqtt():
    """ Connects to the MQTT broker. """

    global client, configDict

    client = mqtt.Client()
    client.connect( configDict['broker'], int(configDict['port']) )



def main():
    """ Main function. Readings the config, connects the broker, and sends the occupancy."""

    global client, configDict, timeLastMessage

    configDict = readConfig() # Read the config file, put in dictionary
    connect_mqtt() # Connect to the MQTT broker


    prevOccupancy = None
    timeLastMessage = 0

    while True: # Main loop, loop forever
        curOccupancy = get_occupancy()
        
        if curOccupancy != prevOccupancy or time.time() - timeLastMessage >= 60:
            send_occupancy(curOccupancy)
            print(curOccupancy)

        prevOccupancy = curOccupancy
        
        time.sleep(2)



if __name__ == "__main__":
    main()
