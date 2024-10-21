# Sensors-SmartInstall

This repository contains all microcontroller sketches and independent scripts created and tested by the Sensors Team for the ERDC VERSA Smart Installation Project.

Documentation over the Data Manager, the flask server that supplements the Sensor Networks, can be found here: [SensorsAPI-SmartInstall](https://cs-utulsa.github.io/SensorsAPI-SmartInstall/#/)

## Team

All contributions on this GitHub have been made by the former and current primaries of the Sensors Team:

* Eliga Franks
* Sterling Fisher
* Nicholas Chorette
* Daniel Webster

For any questions about a repository file should be directed towards the team member(s) who contributed to the file in question.

## Sensors Overview

The Sensors Team's primary goal is to work with static sensing devices that can observe data that ties into policy violations within a given location.

In any given location, we have a set of sensors, reading information about different parts of the room. The sensors are grouped by location and attached to a nearby Sensor Hub (often a simple microcontroller), which gathers the sensor readings and sends the data to a central computer, referred to as the Aggrigator. Some sensors are independent and will act as their own Sensor Hub, sending their data directly to the Aggrigator.

We primarily utilize the MQTT (Message Queuing Telemetry Transport) protocol to gather the readings through a wireless network, but we also utilize subnetworks that connect the Aggrigator and the primary network to Sensor Hubs through other communication protocols.

## Repository Overview

This repository stores both the code for the Sensor Hubs and for the Aggrigator scripts. We primarily utilize the Arduino programming language (based in C++) for our Sensor Hubs, and Python for our aggrigation scripts.

The repository is split into 2 major categories, with a folder for each category:

* **MQTT:** All files related to the primary communication protocol for the sensors. This includes both the scripts used on the Aggrigator (referred to as a "broker" in MQTT) as well as the programs flashed to the Sensor Hubs to collect and send the data through this network.
* **Bluetooth:** All files related to using the BLE communciation protocol as a subnetwork to connect Sensor Hubs to the primary network and the aggrigator.

# Architecture Overview

The following diagram explains the general architecture of the sensors within a constellation, from the devices themselves to Knowledge Core:

A few things to note:

* Each **arrow** represents the flow of data, starting at the boards and going to either the local constellation database or to the Knowledge Core's servers.
  * MQTT Command communication has been omitted in favor of displaying the flow of *sensor readings* and *OTA update* files.
* Each **oval** represents a collection of devices.
* Each **rectangle** represents a collection of software, scripts, and/or servers which can be stored independently from each other on a device.
* Each color represents a seperate communication protocol. **Purple** represents BLE communication, **green** represents MQTT communication, and **red** represents HTTP communication.
  * A color-coded rectangle represents a host/server for the corresponding communication protocol.
  * A color-coded arrow shows what communication protocol was used and what operation was used to communicate the data.

![Attachment](attachments/Sensors%20Architectual%20Diagram.png)
