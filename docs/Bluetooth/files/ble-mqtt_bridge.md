# ble-mqtt_bridge

## Purpose

The purpose of this Python script is to create a bridge between multiple devices running the BLE communication protocol and the MQTT network.

## Overview

This script acts as the central logic to the BLE aggregator.

First, the script utilizes the *asyncio* Python library to spin up asynchronous processes, one process per each valid device. Then, the process tries to connect to its assigned process, and when it does, it begins to read the process's broadcasted data.

After reading one iteration of the broadcasted data of its assigned device, the process then sends the data through MQTT as readings.

## Code Highlights

### Method: start_async()

This method handles most of the functionality of the code, creating asynchronous method calls with the *asyncio* library. This method is apart of the main thread of the program, while the methods called asynchronously here creates new threads with new tasks for the program.

On loop, this method creates an async task that scans for BLE devices, and waits 10 seconds for the scan to finish and add the discovered devices to the `subscribableDevices` list. Then, for each valid device (see `isValidDevice` method for more information) it creates a new asynchronous thread to handle the connection to and data collecting from that device.

### Method: publish_device()

This asynchronous method connects to the given device, subscribes to its services and characteristics, and then collects data from it until disconnection or an error.

On loop, this method grabs an *asynchronous lock* to make sure the connection process doesn't conflict with the connection of other devices on other threads. Without this lock, only one device can be connected at a time, with the other devices failing to connect.

The method connects to the device, ensures that the connection was successful, and then subscribes to each service and their corresponding chracteristics using the `subscribe_service()` method. If there is any issue with this connection process, it tries again up until 5 connection attempts. After that, it calls the `read_data()` method to retrieve the data from the device characteristics.

### Method: subscribe_service()

### Method: read_data()



## Notable Points

* It requires further testing to see just how many BLE devices the bridge can connect to at one time, with the current maximum devices tested being 3.
