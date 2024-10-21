

#include <ArduinoBLE.h>

#include "config.h"

// Enter in Config
  char deviceName[] = BLE_NAME;


// BLE Variables

  String bleName;


  BLEService rssiService("19B10010-E8F2-537E-4F6C-E104768A1214"); // create service

  BLECharacteristic firstRssiCharacteristic("19B10012-E8F2-537E-4F6C-E104768A1214",
                                          BLERead | BLENotify, 32 * sizeof(char));
  
  BLECharacteristic secondRssiCharacteristic("29B10012-E8F2-537E-4F6C-E104768A1214",
                                        BLERead | BLENotify, 32 * sizeof(char));

  BLECharacteristic thirdRssiCharacteristic("39B10012-E8F2-537E-4F6C-E104768A1214",
                                          BLERead | BLENotify, 32 * sizeof(char));
  
  BLECharacteristic fourthRssiCharacteristic("49B10012-E8F2-537E-4F6C-E104768A1214",
                                        BLERead | BLENotify, 32 * sizeof(char));



  // String knownAddresses[64];
  // String knownNames[64];
  // int knownRssi[64];




void setup() {
  // Init function, runs before anything else

//Initialize serial communication and wait for port to open:
  Serial.begin(9600); // Attempt to connect to serial communication

  while(!BLE.begin()) {
    Serial.println(F("Bluetooth Low Energy failed! Trying again..."));
  }

  BLE.setLocalName(bleName);
  BLE.advertise();

  Serial.println("Scanning:");
  BLE.scan();
  Serial.println("Scanning Complete!");

}




void initializeBLE() {
  // Initialize the BLE functionality, and set services and corresponding characteristics
  
  while(!BLE.begin()) { // begin initialization
    Serial.println("starting Bluetooth® Low Energy module failed!");
  }

  String address = BLE.address();
  address.toUpperCase();

  bleName = deviceName;
  bleName += ":";
  bleName += address[address.length() - 5];
  bleName += address[address.length() - 4];
  bleName += address[address.length() - 2];
  bleName += address[address.length() - 1];

  BLE.setLocalName(bleName.c_str()); // set the local name peripheral advertises

  // Manage services and service characteristics  
  BLE.setAdvertisedService(rssiService);

  // Add all the previously defined Characteristics
    rssiService.addCharacteristic(firstClosestCharacteristic);
    rssiService.addCharacteristic(secondClosestCharacteristic);
    rssiService.addCharacteristic(thirdClosestCharacteristic);
    rssiService.addCharacteristic(fourthClosestCharacteristic);
  
  BLE.addService(rssiService); // add the service
  
  BLE.advertise(); // start advertising
  
  Serial.println("Scanning...");
  BLE.scan();

}




void loop() {
  // put your main code here, to run repeatedly:

  BLE.poll();

  if(intervalCheck()) {  // If it is time to read data again
    previousMillis = currentMillis; // Saves last time message was sent

    bleHandler();

  }


}


void scanHandler() {
  BLEDevice device = BLE.available();

  if(device) {

    if(device.hasLocalName()) {
      char* characteristicOut = device.localName() + "-" + device.address() + ": " + device.rssi();
    }


    char* characteristicOut = 
  }
}


void bleHandler() {





  
  char* characteristicOut = "test"; // Characteristic sends ascii value
  Serial.print(characteristicOut);

  exampleCharacteristic.writeValue(characteristicOut);  // Write value to characteristic

}



bool intervalCheck() {
  currentMillis = millis();
  return currentMillis - previousMillis >= interval;
}
