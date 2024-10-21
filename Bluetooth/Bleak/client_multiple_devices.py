# serviceUuid = "19b10000-e8f2-537e-4f6c-d104768a1214"
# characteristicUuid = "19b10001-e8f2-537e-4f6c-d104768a1214"

import asyncio
from bleak import BleakScanner, BleakClient
from bleak.backends.device import BLEDevice

import paho.mqtt.client as mqtt

import contextlib

import time

address = "c3:51:d4:7c:06:ae"

subscribedDevices = []
subscribableDevices = []

# ledGestureServiceUuid = "19b10010-e8f2-537e-4f6c-d104768a1214"

# ledCharacteristicUuid = "19b10011-e8f2-537e-4f6c-d104768a1214"
# gestureCharacteristicUuid = "19b10012-e8f2-537e-4f6c-d104768a1214"

serviceData = {
        "19b10010-e8f2-537e-4f6c-d104768a1214" : {
            "name" : "ledGestureService",
            "characteristics" : {
                #"19b10011-e8f2-537e-4f6c-d104768a1214" : "led",
                "19b10012-e8f2-537e-4f6c-d104768a1214" : "gesture"
            }
        },

        "19b10000-0000-537e-4f6c-d104768a1214" : {
            "name" : "niclaService",
            "characteristics" : {
                #"19b10000-1001-537e-4f6c-d104768a1214" : "version",
                "19b10000-2001-537e-4f6c-d104768a1214" : "temperature",
                "19b10000-3001-537e-4f6c-d104768a1214" : "humidity",
                "19b10000-4001-537e-4f6c-d104768a1214" : "pressure",
                "19b10000-5001-537e-4f6c-d104768a1214" : "accelerometer",
                "19b10000-6001-537e-4f6c-d104768a1214" : "gyroscope",
                "19b10000-7001-537e-4f6c-d104768a1214" : "quaternion",
                #"19b10000-5001-537e-4f6c-d104768a1214" : "rgbLed",
                "19b10000-9001-537e-4f6c-d104768a1214" : "bsec",
                "19b10000-9002-537e-4f6c-d104768a1214" : "co2",
                "19b10000-9003-537e-4f6c-d104768a1214" : "gas"
            }
        }
}

#nameParams = ["Nano 3"]
#identifier = "MQTT:"
identifier = "BLE:"

def isValidDevice(device : BLEDevice) -> bool:

    if device.name == None:
        return False
    
    return identifier in device.name



async def connect_to_device(lock: asyncio.Lock, mqttClient : mqtt.Client, device: BLEDevice):

    while True:

        print("Starting connection to device")
        try:
            async with contextlib.AsyncExitStack() as stack:
                async with lock:
                    client = BleakClient(device)

                    print("connecting to device: " + str(device.name))
                    await stack.enter_async_context(client)
                    print("Connected to device: " + str(device.name))
                    subscribedDevices.append(device.name)

                await asyncio.sleep(5)

                services_object = client.services

                subscribedCharacteristics = []
                
                for serviceID in services_object.services:
                    #await asyncio.sleep(1)
                    service = services_object.services[serviceID]
                    #await asyncio.sleep(4)

                    if service.uuid in serviceData.keys():
                        print("Service is in!")
                        print("Service: " + serviceData[service.uuid]["name"])
                        for characteristic in service.characteristics:
                            #print("Characteristic: " + str(characteristic.uuid))
                            if characteristic.uuid in serviceData[service.uuid]["characteristics"].keys():
                                print("Subscribing to characteristic: " + serviceData[service.uuid]["characteristics"][characteristic.uuid])
                                subscribedCharacteristics.append( (characteristic.uuid, serviceData[service.uuid]["characteristics"][characteristic.uuid]) )
                    
                print("All characteristics subscribed!")

                decodedMessage = ""
                prevMessage = ""
            

                while True:
                    for characteristicTuple in subscribedCharacteristics:
                        characteristicUuid = characteristicTuple[0]
                        byteMessage = await client.read_gatt_char(characteristicUuid)
                        #print("byte Message: ", byteMessage)
                        #print(f"Characteristic: {0}".format("".join(map(chr, byteMessage))))
                        #print(byteMessage.hex())
                        #decodedMessage = f"{0}".format("".join(map(chr, byteMessage)))
                        #print(str(characteristicTuple[1]) + " Reading " + decodedMessage)
                        
                        #decodedMessage = str(int.from_bytes(byteMessage, byteorder='little', signed=False))
                        decodedMessage = byteMessage.decode('ascii')
                        #decodedMessage = byteMessage.decode("utf-8")
                        #decodedMessage = str(float.fromhex(byteMessage.hex()))

                        if(decodedMessage != prevMessage):
                            #print(str(characteristicTuple[1]) + " Reading: {0}".format(decodedMessage))
                            print(str(characteristicTuple[1]) + " Reading " + decodedMessage)
                            topicString = characteristicTuple[1] + "/" + str(device.name)
                            mqttClient.publish(topicString, str(decodedMessage))
                            print("Topic published!")

                            mqttClient.publish("rssi/" + str(device.name), device.rssi)

                        prevMessage = decodedMessage

                                
                                #print("Services:" + str(services_object.services))
        except Exception as e:
            print("Exception: " + str(e))
            print("Exception in connect_to_device: " + str(device.name))
    





async def scan_devices():
    devices = await BleakScanner.discover()
    for d in devices:
        if(isValidDevice(d) and d.name not in subscribedDevices):
            subscribableDevices.append(d)
            print(d)




async def start_async(mqttClient : mqtt.Client):

    lock = asyncio.Lock()
    while True:
        asyncio.create_task(scan_devices())
        await asyncio.sleep(10)

        if subscribableDevices != []:
            asyncio.create_task(connect_to_device(lock, mqttClient, subscribableDevices.pop()))
        
        await asyncio.sleep(10)

    #await asyncio.gather(*[connect_to_device(lock, mqttClient, device) for device in devices if isValidDevice(device)])


if __name__ == "__main__":
    mqttClient = mqtt.Client()
    mqttClient.connect("192.168.2.68", 1883)
    #mqttClient.connect("10.0.2.154", 1883)

    asyncio.run(start_async(mqttClient))