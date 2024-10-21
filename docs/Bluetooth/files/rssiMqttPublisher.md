# rssiMqttPublisher

## Purpose

The purpose of this Arduino sketch is to gather the BLE signal strength between the host device (MKR WiFi 1010) and any other advertising devices in range, and then publish the data through MQTT.

## Overview

This sketch can be split up into two distinct phases, of which it loops though:

* **Bluetooth:** The device performs a BLE scan of any advertising nearby devices, and builds up a JSON file including all of the RSSI data between it and the other devices. This happens for a set interval of time.
* **WiFi:** The device sends the RSSI JSON file through WiFi using MQTT. When this operation is finished, it restarts the BLE scanning.

The major issue with this sketch, due to the way the MKR WiFi 1010 is manufactured, the WiFi and BLE functionality is mutually exclusive. As such, during the Bluetooth phase, the WiFi drivers must be disabled, and visa versa.

This sketch is relatively slow, taking about 15-30 seconds to send the RSSI data through MQTT. This makes it difficult to use for the triangulation use case that it was originally designed for.
