
#include <ArduinoBLE.h>

void setup() {
  // put your setup code here, to run once:

  if(!BLE.begin())
    Serial.println("Bluetooth start failed!");

  BLE.setLocalName("BLE:BAAA");
  BLE.advertise();

  

}

void loop() {
  // put your main code here, to run repeatedly:
  BLE.poll();
}
