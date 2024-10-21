# Data Information

## Data Types

As stated in the [MQTT Overview](MQTT/mqtt.md), the data is organized with topics representing the different reading types available through the Sensor Network.

The following lists each type of reading, with the *topic name* on the left and the unit of measurement and the description (if necessary) on the right.

### Standard (MKR IoT Carrier Rev2)

* `temperature` -> celsius
* `humidity` -> percent
* `pressure` -> hectopascal
* `gas_resistor` -> Unknown; most likely a measurement from which the VOC and CO2 data get extracted from.
* `VOC` -> ppm (parts per million); measures the approximate amount of volatile organic compounds (VOC) within the air.
* `CO2` -> ppm (parts per million)
* `acceleration` -> v3; represents the acceleration of the device on 3 axes, relative to the force of gravity (g).

### Nicla Sense ME

* `temperature` -> celsius
* `quaternion` -> quaternion; angle of the board
* `co2` -> ppm (parts per million)
* `accelerometer` -> m/s2
* `pressure` -> hPa
* `gas` -> ohm
* `bsec` -> bsec; A specialized measurement from which other gas information is extracted from.
* `humidity` -> percent
* `gyroscope` -> dps

### **Specialized**

* `pir_motion` -> flag; simply returns *true* if motion occurs in the sensor's field of view.
* `occupancy` -> number; The number of people considered inside the room at the time (number of people in - number of people out).
* `heartbeat` -> flag; simply returns true if the monitored device is powered.
