# occupancy_mqtt

*This file is now **depreciated**, being used as the base for the more generalized mqtt_api_bridge script.*

## Purpose

The purpose of this Python script is to retrieve the occupancy data from the SenSource occupancy sensor and send it through the MQTT network.

## Overview

While the Sensource occupancy sensor is held within Rayzor 2015, most of the data processing happens within the SenSource servers. Thus, this script makes a GET request to the SenSource API, and then publishes it on the sensor network.

This file is ran on the central computer as a Docker Container using the *Dockerfile* within the same directory as the script.

## Notable Points

- You must have a *config.ini* file in a format shown in *config_example.ini*.
- You must of internet access to retrieve the data from the API.
- This script uses the **Paho** library to handle the connection to the Broker, so it must be imported before continuing. You can use the *requirements.txt* file to download the necessary library.
