# MkrMqttInit

## Purpose

The purpose of this Arduino sketch is to initialize and setup the MKR 1010 Boards within the main Sensors network.

## Overview

This sketch is a stripped-down version of the MkrMqttPublisher sketch, which only initializes any variables stored on the SD card and then connects the device to the Flask server to OTA download the latest version of the corresponding sketch.
