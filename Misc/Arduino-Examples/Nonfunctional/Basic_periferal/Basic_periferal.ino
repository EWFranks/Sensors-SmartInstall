#include <ArduinoBLE.h>
//Added Carrier import
#include <Arduino_MKRIoTCarrier.h>
MKRIoTCarrier carrier;
BLEService newService("180A"); // creating the service

BLEUnsignedCharCharacteristic randomReading("2A58", BLERead | BLENotify); // creating the Analog Value characteristic
BLEByteCharacteristic switchChar("2A57", BLERead | BLEWrite); // creating the LED characteristic

//const int ledPin = 2;
long previousMillis = 0;
//Some basic color usage
uint32_t colorRed = carrier.leds.Color(200, 0, 0);
uint32_t colorGreen = carrier.leds.Color(0, 200, 0);
uint32_t colorBlue = carrier.leds.Color(0, 0, 200);

void setup() {
  Serial.begin(9600);    // initialize serial communication
  while (!Serial);       //starts the program if we open the serial monitor.
  CARRIER_CASE = false;
  if (!carrier.begin())
  {
    Serial.println("Carrier not connected, check connections");
    while (1);
  } 
  //pinMode(LED_BUILTIN, OUTPUT); // initialize the built-in LED pin to indicate when a central is connected
  //pinMode(ledPin, OUTPUT); // initialize the built-in LED pin to indicate when a central is connected

  //initialize ArduinoBLE library
  if (!BLE.begin()) {
    Serial.println("starting Bluetooth® Low Energy failed!");
    while (1);
  }
  carrier.leds.fill(carrier.leds.Color(0, 0, 0), 0, 5);
  carrier.leds.setPixelColor(1, colorGreen); // Shows periferal action
  carrier.leds.show();

  BLE.setLocalName("MKR WiFi 1010 (Periferal Trial)"); //Setting a name that will appear when scanning for Bluetooth® devices
  BLE.setAdvertisedService(newService);

  newService.addCharacteristic(switchChar); //add characteristics to a service
  newService.addCharacteristic(randomReading);

  BLE.addService(newService);  // adding the service

  switchChar.writeValue(0); //set initial value for characteristics
  randomReading.writeValue(0);

  BLE.advertise(); //start advertising the service
  Serial.println(" Bluetooth® device active, waiting for connections...");
}

void loop() {
  
  BLEDevice central = BLE.central(); // wait for a Bluetooth® Low Energy central

  if (central) {  // if a central is connected to the peripheral
    
    Serial.print("Connected to central: ");
    
    Serial.println(central.address()); // print the central's BT address
    
    //digitalWrite(LED_BUILTIN, HIGH); // turn on the LED to indicate the connection
    carrier.leds.setPixelColor(4, colorBlue);
    carrier.leds.show();
    // check the battery level every 200ms
    // while the central is connected:
    while (central.connected()) {
      //long currentMillis = millis();
      
      //if (currentMillis - previousMillis >= 200) { // if 200ms have passed, we check the battery level
        //previousMillis = currentMillis;

        //int randomValue = analogRead(A1);
        //randomReading.writeValue(randomValue);

        //if (switchChar.written()) {
          //if (switchChar.value()) {   // any value other than 0
            //Serial.println("LED on");
            //digitalWrite(ledPin, HIGH);         // will turn the LED on
          //}// else {                              // a 0 value
            //Serial.println(F("LED off"));
          //  digitalWrite(ledPin, LOW);          // will turn the LED off
        //  }
      //  }

    //  }
    Serial.print("Central processing: ");
    delay(1000);
    }

    
    //digitalWrite(LED_BUILTIN, LOW); // when the central disconnects, turn off the LED
    carrier.leds.setPixelColor(4, colorRed);
    carrier.leds.show();
    Serial.print("Disconnected from central: ");
    Serial.println(central.address());
  }
}