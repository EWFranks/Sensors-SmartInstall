/*
Ok, I want this version of the carrier/wifi to be able to switch between periferal and central roles
*/

#include <Arduino_MKRIoTCarrier.h>
#include <ArduinoBLE.h>

MKRIoTCarrier carrier;

// Buttons stuff
int mode = 0;
uint32_t colorRed = carrier.leds.Color(200, 0, 0);
uint32_t colorGreen = carrier.leds.Color(0, 200, 0);
uint32_t colorBlue = carrier.leds.Color(0, 0, 200);

//BLE setup
const char* deviceServiceUuid = "19b10000-e8f2-537e-4f6c-d104768a1214";
const char* deviceServiceCharacteristicUuid = "19b10001-e8f2-537e-4f6c-d104768a1214";

BLEService newService(deviceServiceUuid); // creating the service

void setup() {
  
  CARRIER_CASE = false;
  Serial.begin(9600);
  delay(1500);


  if (!carrier.begin())
  {
    Serial.println("Carrier not connected, check connections");
    while (1);
  } 
    //initialize ArduinoBLE library
  if (!BLE.begin()) {
    Serial.println("starting Bluetooth® Low Energy failed!");
    while (1);
  }
  BLE.setLocalName("MKR WiFi 1010 (multi 2)");
  BLE.setAdvertisedService(newService);
  BLE.addService(newService);
  
}

void loop() {
  
  // Check state: 1 = central, 2 = periferal, 3 = dual
  
  carrier.Buttons.update();
      if (carrier.Buttons.onTouchDown(TOUCH0))
    {
      Serial.println("Central mode (1) selected.");
      carrier.display.fillRect(90, 160, 100, 60, 0x00);
      carrier.display.setCursor(95, 170);
      carrier.display.print("00");
      mode = 1;
      carrier.leds.fill(carrier.leds.Color(0, 0, 0), 0, 5);
      carrier.leds.setPixelColor(0, colorGreen);
    }
    if (carrier.Buttons.onTouchUp(TOUCH1))
    {
      Serial.println("Periferal mode (2) selected.");
      carrier.display.fillRect(90, 160, 100, 60, 0x00);
      carrier.display.setCursor(95, 170);
      carrier.display.print("01");
      mode = 2;
      carrier.leds.fill(carrier.leds.Color(0, 0, 0), 0, 5);
      carrier.leds.setPixelColor(1, colorGreen);
    }
    if (carrier.Buttons.getTouch(TOUCH2))
    {
      Serial.println("Dual mode (3) selected.");
      carrier.display.fillRect(90, 160, 100, 60, 0x00);
      carrier.display.setCursor(95, 170);
      carrier.display.print("02");
      mode = 3;
      carrier.leds.fill(carrier.leds.Color(0, 0, 0), 0, 5);
      carrier.leds.setPixelColor(2, colorGreen);
    }
    if (carrier.Buttons.onTouchUp(TOUCH3))
    {
      Serial.println("Touching Button 3");
      carrier.display.fillRect(90, 160, 100, 60, 0x00);
      carrier.display.setCursor(95, 170);
      carrier.display.print("03");
      carrier.leds.setPixelColor(3, colorGreen);
    }
    if (carrier.Buttons.onTouchDown(TOUCH4))
    {
      Serial.println("Touching Button 4");
      carrier.display.fillRect(90, 160, 100, 60, 0x00);
      carrier.display.setCursor(95, 170);
      carrier.display.print("04");

      carrier.leds.setPixelColor(4, colorGreen);
    }
    carrier.leds.show();
    delay(150);
  //Run the bit based on the mode
  switch (mode) {
    case 1:
        // Code to execute when mode is 1
        Serial.println("Beginning Central Processing");
        central();
        break;
    case 2:
        // Code to execute when mode is 2
        Serial.println("Beginning Periferal Advertising");
        BLE.advertise();
        periferal();
        break;
    case 3:
        // Code to execute when mode is 3
        dual();
        break;
    default:
        // Code to execute when mode doesn't match any case
        break;
  }
}
void central() {
  // If not full (not ready yet)
  // Scan for matching devices
  carrier.leds.setPixelColor(4, colorGreen);
  carrier.leds.setPixelColor(3, colorGreen);
  carrier.leds.show();
  BLEDevice peripheral;
  Serial.println("Scanning for Periferal 1");
    do
  {
    BLE.scan();
    peripheral = BLE.available();
  } while (!peripheral);
  // Process connected devices
    if (peripheral.connect()) {
    Serial.println("* Peripheral device found!");
    Serial.print("* Device MAC address: ");
    Serial.println(peripheral.address());
    Serial.print("* Device name: ");
    Serial.println(peripheral.localName());
    Serial.print("* Advertised service UUID: ");
    Serial.println(peripheral.advertisedServiceUuid());
    Serial.println(" ");
    BLE.stopScan();
    carrier.leds.setPixelColor(3, colorBlue); 
    carrier.leds.show();  
  }
    BLEDevice peripheral2;
  Serial.println("Scanning for Periferal 2");
    do
  {
    BLE.scanForUuid(deviceServiceUuid, true);
    peripheral2 = BLE.available();
  } while (!peripheral2);
  // Process connected devices
    if (peripheral2.connected()) {
    Serial.println("* Peripheral device found!");
    Serial.print("* Device MAC address: ");
    Serial.println(peripheral2.address());
    Serial.print("* Device name: ");
    Serial.println(peripheral2.localName());
    Serial.print("* Advertised service UUID: ");
    Serial.println(peripheral2.advertisedServiceUuid());
    Serial.println(" ");
    BLE.stopScan();
    carrier.leds.setPixelColor(4, colorBlue);
    carrier.leds.show();   
  }
  // Send data to knowledge core
}
void periferal() {
  // Check if connected to a central/dual
  // listen for Bluetooth® Low Energy peripherals to connect:
  carrier.leds.setPixelColor(3, colorGreen);
  Serial.print("Checking for central\n");
  BLEDevice central = BLE.central();
  while(!central){ 
    Serial.print("Checking for central\n");
    central = BLE.central();
    delay(1000);
  }
  Serial.print("got it!");
    // if a central is connected to peripheral:
    if (central) {
      
      Serial.print("Connected to central: ");
      // print the central's MAC address:
      Serial.println(central.address());
      carrier.leds.setPixelColor(3, colorBlue);
    }
    // If not, advertize and store data
    // If so, send data until empty and clear backlog
}
void dual() {
  // Run Central, but send data to central instead of to Knowledge Core
  // Run Periferal
  // Potentially keep track of level?
}
