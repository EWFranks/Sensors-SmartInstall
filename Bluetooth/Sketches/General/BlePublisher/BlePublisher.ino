#include <ArduinoBLE.h>

#include "config.h"


// Enter in Config
  char bleName[] = BLE_NAME;

// Interval Variables
  const long interval = 4000; // Interval for sending messages in milliseconds
  unsigned long currentMillis = 0;
  unsigned long previousMillis = 0;

// BLE Variables
  BLEService exampleService("19B10010-E8F2-537E-4F6C-D104768A1214"); // create service

  BLECharacteristic exampleCharacteristic("19B10012-E8F2-537E-4F6C-D104768A1214",   // Create characteristic
                                          BLERead | BLENotify, "00000000000000000000000000000000");



void setup() {
  // Runs at start of sketch, initializes functionality.
  
  Serial.begin(9600);
  initializeBLE();

}


void initializeBLE() {
  // Initialize the BLE functionality, and set services and corresponding characteristics

  
  while(!BLE.begin()) { // begin initialization
    Serial.println("starting Bluetooth® Low Energy module failed!");
  }

  BLE.setLocalName(bleName); // set the local name peripheral advertises


  // Manage services and service characteristics  
  BLE.setAdvertisedService(exampleService); // set the UUID for the service this peripheral advertises:
  exampleService.addCharacteristic(exampleCharacteristic); // add the characteristic(s) to the service
  
  BLE.addService(exampleService); // add the service
  exampleCharacteristic.writeValue("0"); // Set initial value

  
  BLE.advertise(); // start advertising
  Serial.println("Bluetooth® device active, waiting for connections...");

}


void loop() {
  
  BLE.poll(); // poll for BLE events

    if(intervalCheck()) {  // If it is time to read data again
    previousMillis = currentMillis; // Saves last time message was sent

    bleHandler();

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
