#read_characteristic = "19b10000-e8f2-537e-4f6c-d104768a1214"
#write_characteristic = "19b10001-e8f2-537e-4f6c-d104768a1214"

# import asyncio
# from bleak import BleakScanner

import asyncio
from bleak import BleakScanner, BleakClient

async def main():
    devices = await BleakScanner.discover()
    for d in devices:
        print(d)

asyncio.run(main())



address = "C3:51:D4:7C:06:AE"
#MODEL_NBR_UUID = "00002a24-0000-1000-8000-00805f9b34fb"
MODEL_NBR_UUID = "19b10001-e8f2-537e-4f6c-d104768a1214"


async def client_connect():
    async with BleakClient(address) as client:
        if client.is_connected:
            print("connected")
        else:
            print("not connected")

async def main2(address):
    async with BleakClient(address) as client:
        print("yay")
        model_number = await client.read_gatt_char(MODEL_NBR_UUID)
        print("Model Number: {0}".format("".join(map(chr, model_number))))

#asyncio.run(main2(address))
asyncio.run(client_connect())




# import asyncio
# from bleak import BleakClient

# address = "C3:51:D4:7C:06:AE"
# MODEL_NBR_UUID = "19b10000-e8f2-537e-4f6c-d104768a1214"

# async def main(address):
#     async with BleakClient(address) as client:
#         model_number = await client.read_gatt_char(MODEL_NBR_UUID)
#         print("Model Number: {0}".format("".join(map(chr, model_number))))

# asyncio.run(main(address))