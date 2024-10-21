# mqttLogger

*This file is now **depreciated**, with its methods now acting as the base for the Data Manager's MQTT message handling.*

## Purpose

The purpose of this Python script is to receive and log readings from the MQTT network, and then format and send those readings to Knowledge Core.

## Overview

The original intent of this script was to simply take the sensor readings from the network and log it to a file. Now, this script is the central connection between the Sensor Constellation in Rayzor 2015 and Knowledge Core.

This script subscribes to all readings going through the MQTT network, processes that data into a dictionary formatted for Knowledge Core's Rayzor Collector, and then sends that data through an API POST request.

### Receiving Messages

The script receives all readings by acting as a subscriber to the MQTT topic `readings/#`.

To actually receive and use these MQTT messages on its subscribed topic, the program spins up a thread that constantly checks to see if there is a message available that has come through the MQTT network. If there is, it puts the message into a queue for the main thread to process.

This thread-queue system is necessary so that the program does not miss messages or even disconnect because it is too busy processing previous messages.

## Notable Points

- You must have a *config.ini* file in a format shown in *config_example.ini*.
- This script uses the **Paho** library to handle the connection to the Broker, so it must be imported before continuing. You can use the *requirements.txt* file to download the necessary library.
- As stated before, the script subscribes to all devices' readings by using the built-in **wildcard** symbol for the subtopic `readings/#`. This can be changed in the config to subscribe to any topic and subtopic.

## Read More

[How to Use The Paho MQTT Python Client - Beginners Guide](http://www.steves-internet-guide.com/into-mqtt-python-client/)
