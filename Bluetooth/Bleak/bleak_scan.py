import asyncio
from bleak import BleakScanner, BleakClient

async def main():
    devices = await BleakScanner.discover()
    for d in devices:
        print(d)
        try:
            async with BleakClient(d) as client:
                if(client.is_connected):
                    print("Connected")
                    services_object = await client.get_services()

                    for service in services_object:
                        print("Service: " + str(service.uuid))
                        for characteristic in service.characteristics:
                            print("     Characteristic: " + str(characteristic.uuid))
                else:
                    print("Not Connected")
        except Exception as e:
            print("Exception: " + str(e))
            pass
        finally:
            print("Next Device")
            pass


asyncio.run(main())