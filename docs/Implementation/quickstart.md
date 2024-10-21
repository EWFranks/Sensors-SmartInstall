# Quickstart

This guide documents the methodology used to spin up a new instance of the Sensor Network. All of the programs used are managed by Docker as containers, and spun up using a single Docker Compose file.

## Docker Compose

In order to setup the Sensor Team's central computer on a new device and/or for a new network, we can spin up each script or software as a seperate Docker container and utilize a Docker Compose file to spin them up simultaneously.

Located in the root directory of this repository, *docker_compose_example.yaml* spins up 3 seperate containers: one for the [Mosquitto MQTT](MQTT/broker.md) server, one for the [occupancy_mqtt](MQTT/files/occupancy_mqtt) Python script to send the occupancy data, and finally one for the [Data Manager](https://cs-utulsa.github.io/SensorsAPI-SmartInstall/#/).

The example file assumes that it exists in the parent directory of the *Sensors-SmartInstall* and *SensorsAPI-SmartInstall* repository files as well as a Linux directory format, but it can be modified to fit any type of device setup.

## Docker Mounts

Each Docker container used has data that need to persist across instances of the local container. As such, all three Docker containers assume that they have their own folder mounted to the hard drive within `docker_volumes/<container_name>`. This can be adjusted in the Docker Compose.

The mosquitto container assumes that it has the configuration file `mosquitto.conf` within its mounted folder. See the example config file within the *Misc* folder of the repository.

The Data Manager server container assumes a few different files within its mounted folder. The folder `instance_example` on the **SensorsAPI-SmartInstall** repository gives an example of the data needed within the mounted folder.

The `occupancy_mqtt` container needs a config file within its mounted folder. See the example config file within the *occupancy_mqtt* repository folder.

## Steps

As such, the steps to spin up a new instance of the Sensor Network are as follows:

1. Ensure Git and Docker are installed.
2. Pull the Sensors-SmartInstall and SensorsAPI-SmartInstall repositories from GitHub.
3. Copy the Docker Compose example file from within Sensors-SmartInstall outside the repository, and edit the paths to line up to the necessary Dockerfiles and mounts.
4. Copy the example files for each container onto the folder designated to be the mount for the container. Modify for the new IP Address and any other necessary variables.
5. Docker Compose Up. If done correctly, each container should run without needing to immediately restart.
