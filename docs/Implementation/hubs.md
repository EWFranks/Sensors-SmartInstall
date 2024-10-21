# Sensor Hub Information

Each deployed Sensor Hub has a general or specific purpose in the monitoring of a testbed. While we store the data of each deployed Sensor Hub on the Sensors Server (ID, type, zone, etc.), this stored data may not provide enough context to gleam the purpose of the device.

The purpose of this document is to qualitatively describe the purpose, usage, and/or deployment reasoning of the major deployed Sensor Hubs within each testbed. Not all Sensor Hubs will be listed and described here, but the major ones deployed for a specific purpose will be.

Due to the changing nature of the sensors within the testbeds, Sensor Hubs may be added, removed, or changed as needed, and this document may not reflect the most current setup across the Sensor Installations.

## Device Types

**Standard**-> *MKR IoT Carrier Rev2:* These boards contribute to the "general purpose" monitoring of the room. These boards run the [MkrMqttPublisher.ino](MQTT/files/MkrMqttPublisher.md) script, and take about 6 seconds to read all of its sensors and send the readings.

**PIR Motion Sensors** -> *MKR IoT Carrier Rev2 w/ PIR Motion:* These boards ignore most of the functionality of the *MKR IoT Carrier Rev2* to check if it detects motion as quickly as possible. The boards use a modified version of the `MkrMqttPublisher.ino` script named [MkrMqttPIR.ino](MQTT/files/MkrMqttPIR.md).

**Heartbeat** -> *Arduino Uno WiFi Rev2:* These boards monitor the power state of given **host devices** by sending a *heartbeart* message every 30 seconds when the host device is powered on. The boards run the [MqttHeartbeat.ino](MQTT/files/MqttHeartbeat.md) sketch.

**Occupancy Sensor** -> *SenSource Mounted Sensor:* An occupancy sensor above the door only within Rayzor 2015. The device itself actually sends its data to the SenSource API for processing, which we pull on using the [occupancy_mqtt.py](MQTT/files/occupancy_mqtt.md) script.

**Gas BLE Sensor** -> *Nicla Sense ME:* This board records gas and other general-purpose data. This device connects to the MQTT network using the BLE MQTT Bridge script, and takes about 1.5 seconds to send.

## Sensor Hub Specifics

### Rayzor 2015

* **Standard:**

  * `7RPT`
  * `6Q58`
  * `I5IK`
  * `LEM5`
* **PIR Motion Sensors:**

  * `AO7H` -> Next to the door, detects motion when people are entering/leaving the room.
* **Heartbeat:**

  * `2WB9` -> Monitors the TV
  * `XSDP` -> Monitors the left-most computer
  * `K6EJ` -> Montiors the right-most computer
  * `EJ26` -> Monitors the LED Screen
* **Occupancy Sensor:**

  * `Z0R0 `
* **Gas BLE Sensor:**

  * `BLE:NiclaSenseME:CDD5`

### Rayzor 2055

* **Standard:**
  * `M9IF`
  * `G1XE`
* **PIR Motion Sensors:**
  * `FT0S`

### Rayzor 2250

* **Standard:**
  * `JP06`
  * `RLKA`
