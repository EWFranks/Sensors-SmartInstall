# gestureLED

## Purpose

The purpose of this Arduino sketch is to broadcast the Nano 33 BLE Sense gesture sensor data and LED state using BLE communication.

## Overview

This is a simple BLE publishing sketch that was created to demonstrate the capabilities of the BLE subnetwork. As the file name would suggest, this sketch only utilizes the gesture and LED functionality of the board, but can be easily modified to include other sensors on the board.

If a functional example is needed for implementing BLE with Arduinos, this would be a good file to reference.

## Notable Points

Since the BLE-MQTT Bridge script simply reads in the BLE data as ascii characters, the gesture data is written to BLE characteristic as a char array. The LED data is still written as binary data, which means that the bridge would not be able to parse the LED characteristic.
