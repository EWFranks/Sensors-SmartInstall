# -*- coding: utf-8 -*-
"""
Created on Mon Jul 31 09:39:16 2023

@author: hyuga
"""

import matplotlib.pyplot as plt
from matplotlib.patches import Polygon
import matplotlib.image as mpimg

import paho.mqtt.client as mqtt
import time
import datetime
import configparser
import json
import os.path
import requests

file = None
boardDict = {}

def plot_room_with_overlay(room_width, room_height, motion_zones, zone_motion_status, image_path):
    fig, ax = plt.subplots()

    # Load the image to overlay
    img = mpimg.imread(image_path)
    ax.imshow(img, extent=[0, room_width, 0, room_height], alpha=0.5)

    # Plot the motion detection zones as partially transparent polygons for zones with motion detected (True)
    for zone_key, (zone, color) in motion_zones.items():
        if zone_key in zone_motion_status and zone_motion_status[zone_key]:
            if isinstance(zone, Polygon):
                ax.add_patch(zone)
                zone.set_facecolor(color)
                zone.set_alpha(0.5)
            else:
                x, y, width, height = zone
                ax.add_patch(plt.Rectangle((x, y), width, height, color=color, alpha=0.5))

    ax.set_aspect('equal')
    plt.xlabel('X')
    plt.ylabel('Y')
    plt.title('Room Blueprint with Motion Detection Zones and Image Overlay')
    plt.grid(True)
    plt.show()
    


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


def UpdateLocation(messageText, topic):
    """ Generates a dictionary of the POST request data. """

    dict = {}
    
    # Room blueprint dimensions
    room_width = 30
    room_height = 20
    

    # Motion detection zones defined as (shape, color)
    motion_zones = {
        'PIR8': (Polygon([(10, 20), (15, 3), (7, 3), (7, 20)]), 'purple'),  #Zone 1
        'PIR7': (Polygon([(17, 3), (15, 3), (10, 20), (22, 20)]), 'red'),   # Zone 2
        'PIR3': (Polygon([(24, 3), (26, 3), (30, 20), (20, 20)]), 'blue'),  # Zone 3
        'PIR2': (Polygon([(26, 11), (26, 10), (7, 5), (7, 16)]), 'green'),  # Zone A
        'PIR1': (Polygon([(26, 14), (26, 13), (7, 7), (7, 20)]), 'yellow')  # Zone B
    }
    
    
    messageParts = messageText.split(' ')   # Split the message into reading and unit | 25.0 Celsius -> [25.0, Celsius]
    topicParts = topic.split('/') # Split the topic into Device ID and Reading Type | A0A0/temperature -> [A0A0, temperature]
    # boardParts = get_board_data()
    
    #dict['TimeReceived'] = timeReceived
    
    dict["Reading"] = messageParts[0]
    
    #if len(messageParts) > 1:
    #    dict["Unit"] = messageParts[1]
 
    #dict["ReadingType"] = topicParts[0]

    #dict["BoardInfo"] = {}
    #dict["BoardInfo"]["DeviceID"] = topicParts[1]
    if messageParts[0] == "No":
        location[topicParts[1]] = "Empty"
        zone_motion_status[topicParts[1]] = False
    else:
        location[topicParts[1]] = "Occupied"
        zone_motion_status[topicParts[1]] = True
    #if topicParts[1] in boardDict:
        #dict["BoardInfo"]["Building"] = boardDict[topicParts[1]]["Building"]
        #dict["BoardInfo"]["Room"] = boardDict[topicParts[1]]["Room"]
        #dict["BoardInfo"]["Zone"] = boardDict[topicParts[1]]["Zone"]
    # Path to the image file for overlay
    image_path = 'C:/Users/hyuga/OneDrive/Documents/Research/Sensors/PIR Demo/2015_Blueprint.PNG'

    plot_room_with_overlay(room_width, room_height, motion_zones, zone_motion_status, image_path)
    print("Zone 1: " + str(location["PIR8"]) + "  Zone 2: "+ str(location["PIR7"])+"  Zone 3: "+str(location["PIR3"])) 
    
def send_post(dict):
    """ Sends a POST request to the Knowledge Core API. """

    try:
        postRequest = requests.post("http://localhost:5000/api/data", json=dict)
        return postRequest.text
    except requests.exceptions.RequestException:
        print("Error: Could not send POST request to API.")
        return 0



# Thanks to http://www.steves-internet-guide.com/into-mqtt-python-client/
def on_message(client, userdata, message):
    """ Callback method for when a message is received. """

    #print("message received = ", str(message.payload.decode("utf-8")))
    #print("message topic = ", message.topic)
    #print("message qos=", message.qos)
    #print("message retain flag = ", message.retain)

    messageText = str(message.payload.decode("utf-8"))
    topic = message.topic
    #timeReceived = datetime.datetime.now().strftime("%H:%M:%S")

#TODO: Generate POST request
    #post_dict = generate_post(messageText, topic, timeReceived)
    UpdateLocation(messageText, topic)
    
    #print( send_post(post_dict) ) # Send to API, print response

# Write to file (immediately)
    #print("name: " + file.name)
    #file.write("[" + timeReceived + "] " + topic + " : " + messageText + "\n") 
    #file.write(str( post_dict ) + "\n")
    #file.flush() # Write to file immediately
#%%
# Example usage:
if __name__ == "__main__":
   
    location = {"PIR1": "Empty","PIR2": "Empty","PIR3": "Empty", "PIR7": "Empty", "PIR8": "Empty"}
    # Zone motion status (True if motion detected, False otherwise)
    zone_motion_status = {
        'PIR1': False,
        'PIR2': False,
        'PIR3': False,
        'PIR7': False,
        'PIR8': False,
    }
# Read Config File
    configDict = readConfig()
    print(configDict)

# Get Board Data
    #boardDict = get_board_data()

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

        #file = new_file() # Create a new file for current date
        #print("name: " + file.name)

        client.loop_start() # Start receiving messages
        time.sleep(86400) # Continue for 24 hours
        client.loop_stop()

        #file.close() # Close the file, loop to create new file for next day
    # Zone motion status (True if motion detected, False otherwise)
    


