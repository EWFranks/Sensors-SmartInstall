# NiclaSenseBLE

## Purpose

The purpose of this Arduino sketch is to broadcast the sensor readings of the Nicla Sense ME using BLE communication.

## Overview

The Nicla Sense ME is a powerful yet extremely small board that includes many different onboard sensors and BLE capabilites. Since the board does not have Wi-Fi capabilities, it is the perfect use case for the BLE subnetwork.

This sketch is a modified version of the *gestureLED.ino* sketch as well as taking inspiration from the BLE example sketch of the Nicla Sense ME, meant to interact with a web client rather than our script.

## Notable Points

Since the BLE-MQTT Bridge script simply reads in the BLE data as ascii characters, all sensor data is written to BLE characteristic as a char array.
