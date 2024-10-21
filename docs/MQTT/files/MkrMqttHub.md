# MkrMqttHub

## Purpose

The purpose of this Arduino sketch is to:

* Read any sensors attached to an MKR WiFi 1010 board and publish them through MQTT.
* Receive any commands for the board from MQTT and process each command to modify sensor reading behavior.

## Overview

This sketch can seem relatively daunting due to the large amount of files supplementing the sketch. However, the logic is designed to be as modular as possible, abstracting away bits of the code while making sure the base logic can still be followed.

The main file handles the base functionality of the sketch, with the Wi-Fi and MQTT Broker connections and the publishing of sensor data through the network.

The file also initializes the additional C++ logic and objects, allowing the additional functionality to be used in the sketch.

## Additional Logic

### SdConfig.cpp

This file creates a Config object, which wraps a struct of configuration variables that can be loaded from or stored into a file on the SD card.

The configuration variables are as follows:

* `device_id` -> The ID of the device.
* `version` -> The sketch version of the device, used to check and handle OTA updating.
* `mqtt_interval` -> The millisecond delay between the publishing the sensors readings through the MQTT network.
* `carrier_temperature` -> (true/false) Whether the carrier's temperature sensor is gathering readings.
* `carrier_humidity` -> (true/false) Whether the carrier's humidity sensor is gathering readings.
* `carrier_pressure` -> (true/false) Whether the carrier's pressure sensor is gathering readings.
* `carrier_gas` -> (true/false) Whether the carrier's gas sensor is gathering readings.
* `carrier_VOC` -> (true/false) Whether the carrier's VOC sensor is gathering readings.
* `carrier_CO2` -> (true/false) Whether the carrier's CO2 sensor is gathering readings.
* `carrier_acceleration` -> (true/false) Whether the carrier's acceleration sensor is gathering readings.
* `A6` -> The name of the sensor outputting data on A6.
* `A0` -> The name of the sensor outputting data on A0.
* `I2C` -> The name of the sensor communicating the device through I2C.

### MkrCarrierHelper.cpp

This file creates an MkrCarrierHelper object, which wraps the MkrIoTCarrier library and extends it with new functionality and helper methods.

This object takes in a pointer to the Config object defined in SdConfig.cpp and declared in the main file.

### CommandHandler.cpp

This creates a CommandHandler object, which contains the functionality that can be activated by sending the proper commands through the MQTT network.

This object takes in a pointer to the Config object defined in SdConfig.cpp and a pointer to the MkrCarrierHelper object defined in MkrCarrierHelper.cpp, both of which are declared in the main sketch.

Commands usually can take the form of:

```
commands/<Device ID>/<command>
```

with `<Device ID>` being able to be set to any specific Device ID or simply `all_` to refer to all devices on the network.

A list of commands can be found on the *commands* page.

### SensorPinHelper.cpp

This creates a SensorPinHelper object, which helps extract the readings from the sensors attached to the Grove Ports on the Carrier.

The object holds the functionality to read from a selection of sensors, and will use function pointers to switch between each function, to properly read the sensor on the input pin depending on what sensor it is told in the Config object.

This object takes in a pointer to the Config object defined in SdConfig.cpp and a pointer to the MkrCarrierHelper object defined in MkrCarrierHelper.cpp, both of which are declared in the main sketch.

### OtaUpdate.cpp

This handles the OTA mass updating on the Arduino end. It utilizes a modified version of the example found in the *ArduinoOTA* library, and actually doesn't utilize the library at all.

This is accomplished by first checking the sketch version that is saved on the SD card, and comparing that to the most recent version on the Flask server (see the SensorsAPI-SmartInstall documentation). then, if there is a new update, it pulls the proper update file from the server and downloads that to the SD card. Finally, it updates itself through the saved update file, and reboots.

These functions are not within an object, and are just called directly within the base sketch.

## Notable Points

- You must have a *sketch_secrets.h* file with the following fields:
  - *SECRET_SSID* -> Stores the target WiFi network's name.
  - SECRET_USER -> Stores the target WiFi network's username credential, if necessary. Leave blank otherwise.
  - *SECRET_PASS* -> Stores the target network's password.
  - MQTT_USER -> Stores the username used for connection with the MQTT server.
  - MQTT_PASS -> Stores the password for the MQTT server corresponding to the input username.
  - BROKER_IP -> Stores the IP of the MQTT server.
  - BROKER_PORT -> Stores the port on which the MQTT server is hosted.
  - SERVER_IP -> Stores the IP of the Data Manager, the Flask server used for OTA updates.
  - SERVER_PORT -> Stores the port that the Data Manager server is hosted.
- You must have a *init_config.h* file in a format shown in *init_config_example.h*.
- There are a few layers of **failsafes** in the code in case of an issue.
  - Every loop, the Arduino confirms the connection to both the WiFi and MQTT Broker before continuing.
  - If the Arduino detects that it is no longer connected to the MQTT server, it will ensure the connection to the WiFi network and try to reconnect.
  - If this fails 5 times, the Arduino is assumed to have an issue with the drivers and will restart as a last resort.

## Read More

[Connecting MKR WiFi 1010 to a Wi-Fi Network | Arduino Documentation](https://docs.arduino.cc/tutorials/mkr-wifi-1010/connecting-to-wifi-network)

[MKR IoT Carrier Cheat Sheet | Arduino Documentation](https://docs.arduino.cc/tutorials/mkr-iot-carrier/mkr-iot-carrier-01-technical-reference)

[Sending Data over MQTT | Arduino Documentation](https://docs.arduino.cc/tutorials/mkr-wifi-1010/mqtt-device-to-device)
