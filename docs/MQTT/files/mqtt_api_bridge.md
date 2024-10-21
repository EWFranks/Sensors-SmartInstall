# mqtt_api_bridge

## Purpose

The purpose of this Python script is to bridge devices that communicate via API calls to the MQTT network.

## Overview

There are two main types of messages that are sent through the MQTT network: *readings* from a given device, and *commands* to the device.

This script acts as a client to the MQTT network, both receiving device *commands* and calling the corresponding API endpoints for a device, as well as getting data from the API endpoints on a loop and publishing that as device *readings*.

Devices that utilize this script include the SenSource Occupancy Sensor and the Lifx Smart Lightbulbs.

### device_info.json

All of the devices and their corresponding command and reading definitions can be found and modified in device_info.json.

In the JSON, we define 3 objects per device: the *headers* for the API Requests, the *commands* and what requests they trigger, and the information describing the *readings*.

The `commands` object describing the device is relatively simple; each command within it has the MQTT topic as the name, and describes the API Request information.

The `readings` object for the device is a bit more complicated in its structure. The first few variables describe how often to send the readings:

* `reading_interval` describes how often the script calls and builds the readings message to send through MQTT
* `allow_normal_repeats` describes whether or not to normally send a readings message if it is the same as the last message sent for the device.
* If repeats are disabled, `repeat_heartbeat` is the interval after the last message is sent that repeats are sent anyways, to signal that the device is still alive and to send the data to any clients who may have missed the last message.

The `requests` array within `readings` describes each API request needed to get readings. Each request may give multiple readings, so `request_readings` describes each reading that can be obtained from the API request and how to find it within the request response.

## Notable Points

The `request_data` parameter describes the JSON data being sent to the API, and arguments are written or overwritten depending on what is specified in the MQTT command's message body. If `request_data` is an empty array, that means that the API does not return a JSON, but rather the reading itself.
