# MkrMqttPIR

*This file is now **depreciated**, being folded into the MkrMqttHub script as a configurable option*.

## Purpose

The purpose of this Arduino sketch is to read the PIR sensor attached to the MKR WiFi 1010 board to track motion and then publish the readings through MQTT.

## Overview

One of the goals of the Sensors Team is to track the occupancy of a given room. While we have a good occupancy sensor by the door, we can also use PIR sensors to detect motion, contributing to our occupancy goal.

This sketch is a modification of the *MkrMqttPublisher* sketch, stripping away most of the sensors in favor of speeding up the reading and sending of the PIR sensor data.

While this sketch reads from the PIR Sensor every quarter of a second, this sketch is event based, meaning that it will only send when it detects motion, and will send a no motion signal after about a half of a second.

## Notable Points

- You must have an *arduino_secrets.h* file with the *SECRET_SSID* and *SECRET_PASS* fields to store the network's name and password for connection.
- You must have a *config.h* file in a format shown in *config_example.h*.
