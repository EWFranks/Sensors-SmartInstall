
""" This script is used to connect to multiple BLE devices and publish its data to an MQTT broker. """

import asyncio
from bleak import BleakScanner, BleakClient
from bleak.backends.device import BLEDevice

import paho.mqtt.client as mqtt

import contextlib

import os
import json
import configparser

import logging

serviceData = {}

subscribedDevices = [] 
subscribableDevices = []

identifier = "BLE:"





async def start_async(mqttClient : mqtt.Client):
    """ Method to start the asyncronious tasks. """

    connectionLock = asyncio.Lock() 

    while True:
        asyncio.create_task(scan_devices()) # Scan for devices
        await asyncio.sleep(10) # Wait for 10 seconds

        # if there are devices that can be subscribed to, subscribe to them
        if subscribableDevices != []:
            asyncio.create_task(publish_device(connectionLock, mqttClient, subscribableDevices.pop()))
        
        await asyncio.sleep(10)



async def publish_device(lock: asyncio.Lock, mqttClient : mqtt.Client, device: BLEDevice):
    """ Method to connect to a device and subscribe to its characteristics.
        Runs asynchroniously per device being connected to. """
    
    connection_attempts = 0
    while True: # Loop forever -> Marks top of publish loop, reconnect if somehow disconnected
        


        try:
            async with BleakClient(device) as client: # Connect to the device
                async with lock: # Lock the connection loop so that only one device can be connected to at a time
                
                
                    subscribedDevices.append(device.name)
                    
                    await asyncio.sleep(5) # Wait for 5 seconds
                    assert client is not None # Assert that the client is not None

                    services_object = client.services # Get the advertised services from the device
                    subscribedCharacteristics = []

                    for serviceID in services_object.services: # Loop through the services on the device
                        service = services_object.services[serviceID]

                        curServiceCharacteristics = await subscribe_service(service) # Subscribe to the service and its characteristics
                        
                        for curServiceCharacteristic in curServiceCharacteristics:
                            if curServiceCharacteristics not in subscribedCharacteristics:
                                subscribedCharacteristics.append(curServiceCharacteristic)
                            
                        print("All characteristics subscribed!")

                # Lock released
                
                # Loop forever and read data from the device
                await read_data(client, subscribedCharacteristics, device, mqttClient)
                # is read_data not awaited?

        except Exception as e:
            logging.warning("Exception: " + str(e))
            logging.warning("Exception in Connecting To Device: " + str(device.name))
            connection_attempts += 1

            # Remove the device from the subscribed devices list (resubscribing)
            if device.name in subscribedDevices:
                subscribedDevices.remove(device.name) 
            
            if connection_attempts < 5: # Restart the connection loop if there is an exception
                continue 
            else:                       # Return if there are too many connection attempts
                logging.error("Unable to connect to device: " + str(device.name))
                return                  






async def subscribe_service(service) -> list:
    """ Method to subscribe to a service and its characteristics. """

    serviceSubscribedCharacteristics = []

    if service.uuid in serviceData.keys(): # If the service is in the service data, then subscribe to its enabled characteristics
        curServiceData = serviceData[service.uuid]
        
        print("Service: " + curServiceData["name"])

        for characteristic in service.characteristics: # Loop through the characteristics of the service

            if characteristic.uuid in curServiceData["characteristics"].keys(): # If the characteristic is in the service data
                curCharacteristicData = curServiceData["characteristics"][characteristic.uuid]

                # If the characteristic is enabled, subscribe to it
                if curCharacteristicData["enabled"] == True:
                    
                    print("Subscribing to characteristic: " + curCharacteristicData["reading_val_type"])
                    serviceSubscribedCharacteristics.append( (service.uuid, characteristic.uuid) ) # Add a (service,characteristic) uuid tuple to a subscribed list

    return serviceSubscribedCharacteristics



async def read_data(client: BleakClient, subscribedCharacteristics: list, device: BLEDevice, mqttClient: mqtt.Client):
    """ Method to forever read data from the device and publish it to the MQTT broker. """
    
    decodedMessage = ""
    prevMessage = ""

    missedReadings = 1

    topic = f"readings/{device.name}"

    while True: # Read data from the device forever
        try: # Try to do readings, else simply restart the loop

            readings = []

            for characteristicTuple in subscribedCharacteristics: # Loop through the subscribed characteristics of the device
                serviceUuid = characteristicTuple[0]
                characteristicUuid = characteristicTuple[1]

                byteMessage = await client.read_gatt_char(characteristicUuid) # Read the characteristic data in bytes
                decodedMessage = byteMessage.decode('ascii') # Decode the bytes into a string

                if(decodedMessage != prevMessage): # If the message is different from the previous message, then publish it
                    characteristicData = serviceData[serviceUuid]["characteristics"][characteristicUuid]
                
                    print(f"{characteristicData['reading_val_type']} Reading {decodedMessage} {characteristicData['reading_unit']}")

                    readings.append(
                        {
                            "reading_type" : characteristicData["reading_val_type"],
                            "reading_unit" : characteristicData["reading_unit"],
                            "reading_val" : decodedMessage
                        }
                    )

                prevMessage = decodedMessage


            # After all the characteristics have been read, publish the readings
            jsonMessage = {
                "readings" : readings
            }

            if(readings != []):
                mqttClient.publish(topic, json.dumps(jsonMessage))
                print("Readings published!")

            # Publish rssi data seperate
            mqttClient.publish(f"data/{device.name}", device.rssi)
            await asyncio.sleep(.2)

        except Exception as e:
            logging.warning("Exception: " + str(e))
            logging.warning("Exception in reading data: " + str(device.name))

            if missedReadings >= 5:
                print(f"Reconnecting to device: {device.name}")
                return # Return to the connection loop if there are too many missed readings

            missedReadings += 1



async def scan_devices():
    """ Method to scan for available and valid devices. """

    print("Scanning for devices...")

    devices = await BleakScanner.discover()
    for d in devices:
        if(isValidDevice(d) and d.name not in subscribedDevices):
            subscribableDevices.append(d)
            print(d)



def isValidDevice(device : BLEDevice) -> bool:
    """ Method to test if a device is setup to be subscribed to for this project. """

    if device.name == None:
        return False
    
    return identifier in device.name



def getServiceData() -> dict:
    """ Gets the service data from the service_data.json file. """

    #filePath = os.path.join(os.path.dirname(__file__), "Data/service_data.json")
    filePath = os.path.dirname(os.path.realpath(__file__)) + "/instance/data/service_data.json"

    with open(filePath) as json_file:
        return json.load(json_file)
    


def readConfig():
    """ Reads the config file and returns a dictionary with the config data. """

    config = configparser.ConfigParser()

    configPath = os.path.dirname(os.path.realpath(__file__)) + '/instance/config.ini'
    config.read(configPath)


    # FORMAT: dict['IP'] = config['MQTT']['brokerIP']
    dict = {}

    for section in config.sections():                   # For each section
        for option in config.options(section):          # For each option in that section
            dict[option] = config[section][option]      # Add it to the dictionary under that option

    return dict





def main():
    """ Main method. """

    print("Starting!")

    configDict = readConfig()

    mqttClient = mqtt.Client()
    mqttClient.username_pw_set(configDict['mqtt_user'], configDict['mqtt_pass'])

    mqttClient.connect(configDict['mqtt_ip'], int(configDict['mqtt_port'])) # Connect to MQTT server from with config info 

    global serviceData
    serviceData = getServiceData()

    asyncio.run(start_async(mqttClient))



if __name__ == "__main__":
    main()