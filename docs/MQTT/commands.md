# Command Overview

Not only can Arduino boards publish data through the MQTT network, they can also receive data as well.

All deployed Arduino boards connected to the MQTT network are subscribed to the `commands/` main topic, which allows them to receive and carry out commands through the network sent, from an external device.

Commands can be sent to a specific device, or can be sent to all devices on the network.

## MQTT Format

Commands usually can take the form of:

```
commands/<Device ID>/<command>
```

with `<Device ID>` being able to be set to any specific Device ID or simply `all_` to refer to all devices on the network.

Some commands are augmented by the value within the message body.

## Deployed Commands

Different Sensor Hubs can receive and interpret different commands from the MQTT network. The commands available for each device type are as follows:

### Standard (MKR IoT Carrier Rev2)

* `restart` -> Restarts the board(s).
* `jingle` -> Plays a jingle on the board(s).
* `set/<variable>` -> Sets the given parameter on the board.

  * `set/id` -> Sets the Device ID for the board. Requires a restart.
  * `set/directory` -> Sets the server directory that the board searches in for their OTA update file.
  * `set/interval` -> Sets the time between sensor readings on a given device to the given value in milliseconds.
  * `set/carrier_temperature_enabled` -> Sets the boolean (true/false) on whether the carrier temperature sensor is enabled.
  * `set/humidity_enabled` -> Sets the boolean (true/false) on whether the carrier humidity sensor is enabled.
  * `set/carrier_pressure_enabled` -> Sets the boolean (true/false) on whether the carrier pressure sensor is enabled.
  * `set/carrier_gas_enabled` -> Sets the boolean (true/false) on whether the carrier gas sensor is enabled.
  * `set/carrier_VOC_enabled` -> Sets the boolean (true/false) on whether the carrier VOC sensor is enabled.
  * `set/carrier_CO2_enabled` -> Sets the boolean (true/false) on whether the carrier CO2 sensor is enabled.
  * `set/carrier_acceleration_enabled` -> Sets the boolean (true/false) on whether the carrier acceleration sensor is enabled.
  * `set/A6` -> Defines the sensor sending data through pin A6 (*pir*, *water*, etc.)
  * `set/A0` -> Defines the sensor sending data through pin A0 (*pir*, *water*, etc.)
  * `set/I2C` -> Defines the sensor hooked into the I2C port.
* `profile/<profile>` -> Sets predefined configuration settings on the Sensor Hub.

  * `profile/general` -> Sets device configuration to have all carrier sensors enabled, and to send approximately every 6 seconds.
  * `profile/pir` -> Sets device configuration to define a PIR sensor on pin A0 and disable all carrier sensors.

### Heartbeat (Arduino Uno WiFi Rev2)

* `restart` -> Restarts the board(s).
* `set/<variable>` -> Sets the given parameter on the board.
  * `set/id` -> Sets the Device ID for the board. Requires a restart.
  * `set/interval` -> Sets the time between sensor readings on a given device to the given value in milliseconds.
