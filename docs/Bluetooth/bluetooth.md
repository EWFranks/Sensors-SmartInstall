# Bluetooth Overview

While MQTT acts as our main Sensor Network responsible for connecting Sensor Hubs to our Data Manager and to Knowledge Core Servers, it requires that the Sensor Hubs feature wireless connectivity or they must be connected to a device with a Wi-Fi module. To overcome this limitation, we have the concept of Sensor Subnetworks, where boards can connect to our main Sensor Network using a communication protocol other than MQTT and Wi-Fi.

This Sensor Subnetwork utilizes the BLE (Bluetooth Low Energy) communication protocol, which is a fast and reliable protocol for small packets of information. This gives us access to more Sensor Hubs than we would have had otherwise, including the *Arduino Nano 33 BLE Sense* and the *Nicla Sense ME*. After connecting, the Sensor Hubs will broadcast the sensor information, being picked up and translated into the MQTT network by the *Data Manager*.

## Implementation

The BLE Subnetwork implementation can be split up into 2 equal parts.

### Sensor Hubs

Each Arduino sketch follows approximately the same format, with most of the differences between sketches simply a change in the sensors themselves.

Each BLE board broadcasts *services* that have one or more *characteristics* which hold and classify data. For example, the Nicla Sense ME has a service named NiclaService, which contains many different characteristics corresponding to the different sensors on the device (temperature, pressure, CO2, etc.)

Thus, when reading from a sensor, the board can broadcast its data on a characteristic, and any device that reads the characteristic's service can receive and process that data.

### Aggregator

The BLE Aggregator utilizes a Python library named Bleak, which allows us to connect to each BLE-enabled Sensor Hub and read the broadcasted services and their characteristics and data.

The aggregator acts as a bridge between the BLE devices and the MQTT network, receiving the data from the BLE devices, and sending the data through the MQTT network for further processing.
