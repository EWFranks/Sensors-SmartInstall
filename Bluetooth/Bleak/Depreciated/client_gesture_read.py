# serviceUuid = "19b10000-e8f2-537e-4f6c-d104768a1214"
# characteristicUuid = "19b10001-e8f2-537e-4f6c-d104768a1214"

import asyncio
from bleak import BleakScanner, BleakClient

import time

address = "c3:51:d4:7c:06:ae"
#MODEL_NBR_UUID = "00002a05-0000-1000-8000-00805f9b34fb"
#MODEL_NBR_UUID = "19b10001-e8f2-537e-4f6c-d104768a1214"
#MODEL_NBR_UUID = 0xfff0

#ledServiceUuid = "19b10000-e8f2-537e-4f6c-d104768a1214"
ledGestureServiceUuid = "19b10010-e8f2-537e-4f6c-d104768a1214"

#ledCharacteristicUuid = "19b10001-e8f2-537e-4f6c-d104768a1214"
ledCharacteristicUuid = "19b10011-e8f2-537e-4f6c-d104768a1214"
gestureCharacteristicUuid = "19b10012-e8f2-537e-4f6c-d104768a1214"

deviceData = {

    "c3:51:d4:7c:06:ae" : {
        "name": "A0A0",
        "services": {
            "ledGestureServiceUuid":  "19b10010-e8f2-537e-4f6c-d104768a1214",
        },
        
        "characteristics": {
            "led": "19b10011-e8f2-537e-4f6c-d104768a1214",
            "gesture": "19b10012-e8f2-537e-4f6c-d104768a1214"
        }
    }

}


async def main(address):

    devices = await BleakScanner.discover()
    for d in devices:
        print(d)


    async with BleakClient(address) as client:

        if(client.is_connected):
            print("Connected")
            services_object = await client.get_services()
            
            for serviceID in services_object.services:
                service = services_object.services[serviceID]
                
                print("Service: " + str(service.uuid))
                for characteristic in service.characteristics:
                    print("Characteristic: " + str(characteristic.uuid)) 
            
            #print("Services:" + str(services_object.services))
        
        else:
            print("Not Connected")
            return

        prevGestureString = ""

        while True:
            #time.sleep(.5)
            gesture_number = await client.read_gatt_char(gestureCharacteristicUuid)
            gestureString = int.from_bytes(gesture_number, byteorder='little', signed=False)

            if(gestureString != prevGestureString):
                print("Gesture Number: {0}".format(gestureString))

            prevGestureString = gestureString

        #await client.write_gatt_char(ledCharacteristicUuid, bytearray([0x01]), response=False)
        #print("LED ON")
        #time.sleep(2)
        #await client.write_gatt_char(ledCharacteristicUuid, bytearray([0x00]), response=False)
        #print("LED OFF")

asyncio.run(main(address))