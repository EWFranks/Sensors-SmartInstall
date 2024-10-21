import requests
import configparser
from time import sleep
import datetime
import paho.mqtt.client as mqtt

import sys
import queue

import random


import math


localHits = 0
localTotal = 0

mqttHits = 0
mqttTotal = 0

isRunning = False



# Thanks to http://www.steves-internet-guide.com/into-mqtt-python-client/
def on_message(client, userdata, message):
    """ Callback method for when a message is received. """

    global mqttHits, mqttTotal, isRunning

    print("message received = ", str(message.payload.decode("utf-8")))
    print("message topic = ", message.topic)

    messageText = str(message.payload.decode("utf-8"))
    topic = message.topic
    timeReceived = datetime.datetime.now().strftime("%H:%M:%S")

    if(message.topic == "pi/run"):
        if(messageText == "start"):
            isRunning = True
        if(messageText == "end"):
            isRunning = False
    elif("pi/data/" in message.topic):
        messageText = messageText.split(" ")[0].split("/") # Split into hits (idx 0) and total (idx 1)
        mqttHits += int(messageText[0])
        mqttTotal += int(messageText[1])



def piCollection():
    """ Collects data for the Monte Carlo pi estimation. """

    global localHits, localTotal

    x = random.random()
    y = random.random()


    if(x**2 + y**2 <= 1):
        localHits += 1

    localTotal += 1



def main():


    client = mqtt.Client()
    client.connect("192.168.2.238", 1883)


    client.subscribe("pi/data/#")
    client.subscribe("pi/run")

    client.on_message = on_message
        
    client.loop_start() # Start receiving messages
    
    while not isRunning: # Wait for start message
        sleep(1)

    while isRunning:
        piCollection()

    client.loop_stop()

    print("Local: " + str(localHits) + "/" + str(localTotal))
    print("MQTT: " + str(mqttHits) + "/" + str(mqttTotal))

    localPi = 4 * localHits / localTotal
    mqttPi = 4 * mqttHits / mqttTotal
    combinedPi = 4 * (localHits + mqttHits) / (localTotal + mqttTotal)

    localError = 100 * abs(localPi - math.pi) / math.pi
    mqttError = 100 * abs(mqttPi - math.pi) / math.pi
    combinedError = 100 * abs(combinedPi - math.pi) / math.pi

    print("Local estimation of Pi: " + str(localPi))
    print("Estimation percent error: " + str(localError) + "%")
    print('\n')

    print("MQTT estimation of Pi: " + str(mqttPi))
    print("Estimation percent error: " + str(mqttError) + "%")
    print('\n')

    print("Combined estimation of Pi: " + str(combinedPi))
    print("Estimation percent error: " + str(combinedError) + "%")
    print('\n')

    print("Increased accuracy due to MQTT: " + str(localError - combinedError) + "%")


if __name__ == "__main__":
    if not main():
       sys.exit(1)
