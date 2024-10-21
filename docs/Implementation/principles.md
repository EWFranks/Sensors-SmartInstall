# Sensors Principles and Goals

With the creation of the standardized Sensors network structure, our priorities are the following:

* **Scalability:** The network should be able to function the same way for one sensor as it would for hundreds of sensors. Multiple networks can be used to accomodate an increase of scale. Furthermore, the network should be able to accomodate many different types of Sensor Hubs operating using different communication methods.
* **Modularity:** Sensors should be able to be added and removed from the network without interrupting the other sensors on the network. Furthermore, Sensor Hubs should be able to be moved from one network to another and continue to function in a similar fashion as well as able to be identified as the same Sensor Hub.

Balancing these principles with the security and speed of the network informed all design decisions by the Sensors team.

## Implementation

These principles of the Sensors team led to a few different implementation concepts necessary for understanding the system as a whole:

* **Bridging:** Different communication protocols and implementations, including BLE, Serial connections, and HTTP RESTful APIs, are translated and "bridged" into the MQTT network using different programs and scripts. The bridging of these communication protocols may create additional delay in receiving the reading information from sensors, however the cost of the delay comes with the benefits of centralizing the data within the MQTT network.
* **Local Database:** Data pertaining to board information is not stored on the board itself, but rather in a small per-installation database. This database is not meant to be of the scale of the databases of the Knowledge Core, but rather a queryable record of boards and their information. This database also holds a temporary record of recent sensor readings within the installation.
* **Bi-Directional Communication:** Just as Sensor Hubs can communicate their sensor readings to the network, the network can react and send information back to the Sensor Hubs in the form of commands and OTA updates.
