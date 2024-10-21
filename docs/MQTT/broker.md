# Broker

The MQTT Aggregator, also referred to as the "Broker", is a Docker Container created from a configured image of an open-source implementation of MQTT called **Eclipse Mosquitto**.

## Local Configuration

A simple Ubuntu-based laptop is running the docker container on the TUERDC network located in Rayzor Hall, and as such, you shouldn't need to run your own broker software unless you are setting up another network to work with MQTT.

In order to get a local instance of the broker software running, you can modify the generated *mosquitto.conf* file, creating a **listener** on a network's port (defaults to *1883*) and disabling the need for a password file to connect. The deployed *mosquitto.conf* file can be found within the **Docker** folder.

Since we have secured the MQTT network with credentials, we have disabled the field *allow_anonymous* and create password files for each device currently deployed. However, if one is simply spinning up an MQTT server for prototyping and debugging purposes, it is not necessary.

## Docker

Docker is a virtualization software that allows us to create portable software instances called *containers* that can be deployed quickly on most hardware. This is useful when replicating the environment across rooms.

The docker folder contains the configuration file needed to build and deploy a containerized version of the Mosqsuitto MQTT software that operates in a similar fashion to what is setup in Rayzor 2015. Be sure to mount the configuration file in the necessary path of the container.
