Here, you can create a Docker container that holds the Mosquitto MQTT Broker, with a custom config file that is mounted on the host machine.


If you have Docker installed, you can use the following commands, or simply run the .bat file, to install the container.

Before you do, you must find a directory to place the mosquitto.conf file. It is defaulted to C:/dockerVolumeTest/
Be sure to change the directory in the commands or the .bat before proceeding.

######

docker pull eclipse-mosquitto
docker run -it -p 1883:1883 -p 9001:9001 -v C:/dockerVolumeTest/mosquitto.conf:/mosquitto/config/mosquitto.conf -v /mosquitto/data -v /mosquitto/log eclipse-mosquitto