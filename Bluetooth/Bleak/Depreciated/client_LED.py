# serviceUuid = "19b10000-e8f2-537e-4f6c-d104768a1214"
# characteristicUuid = "19b10001-e8f2-537e-4f6c-d104768a1214"

import asyncio
from bleak import BleakScanner, BleakClient

import time

address = "c3:51:d4:7c:06:ae"
#MODEL_NBR_UUID = "00002a05-0000-1000-8000-00805f9b34fb"
#MODEL_NBR_UUID = "19b10001-e8f2-537e-4f6c-d104768a1214"
#MODEL_NBR_UUID = 0xfff0

ledServiceUuid = "19b10000-e8f2-537e-4f6c-d104768a1214"
ledCharacteristicUuid = "19b10001-e8f2-537e-4f6c-d104768a1214"

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

        time.sleep(.5)
        #model_number = await client.read_gatt_char(characteristicUuid)
        #print("Model Number: {0}".format("".join(map(chr, model_number))))

        await client.write_gatt_char(ledCharacteristicUuid, bytearray([0x01]), response=False)
        print("LED ON")
        time.sleep(2)
        await client.write_gatt_char(ledCharacteristicUuid, bytearray([0x00]), response=False)
        print("LED OFF")

asyncio.run(main(address))