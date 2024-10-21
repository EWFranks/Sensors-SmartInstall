# Sensors-SmartInstall

This repository contains all code created and tested by the Sensors Team for the ERDC VERSA Smart Installation Project.

## Team

All contributions on this GitHub have been made by the primaries of the Sensors Team:

* Daniel Webster
* Eliga Franks
* Sterling Fisher
* Nicholas Chorette

For any questions about a repository file should be directed towards the team member(s) who contributed to the file in question.

## Sensors Overview

The Sensors Team's primary goal is to work with static sensing devices that can observe data that ties into policy violations within a given location.

In any given location, we have a set of sensors, reading information about different parts of the room. The sensors are grouped by location and attached to a nearby Sensor Hub (often a simple microcontroller), which gathers the sensor readings and sends the data to a central computer, referred to as the Aggrigator. Some sensors are independent and will act as their own Sensor Hub, sending their data directly to the Aggrigator.

We primarily utilize the MQTT (Message Queuing Telemetry Transport) protocol to gather the readings through a wireless network, but we also utilize subnetworks that connect the Aggrigator and the primary network to Sensor Hubs through other communication protocols.

## Repository Overview

This repository stores both the code for the Sensor Hubs and for the Aggrigator scripts. We primarily utilize the Arduino programming language (based in C++) for our Sensor Hubs, and Python for our aggrigation scripts.

The repository is split into 3 different folders:

* MQTT: All files related to the primary communication protocol for the sensors. This includes both the scripts used on the Aggrigator (referred to as a "broker" in MQTT) as well as the programs flashed to the Sensor Hubs to collect and send the data through this network.
* Bluetooth: All files related to using the BLE communciation protocol as a subnetwork to connect Sensor Hubs to the primary network and the aggrigator.
* Examples: Example programs for working with the Sensor Hubs, that are unrelated to the sensor network itself. These files could be used to test gathering data with a new Sensor Hub, or can be used to test a new feature.
