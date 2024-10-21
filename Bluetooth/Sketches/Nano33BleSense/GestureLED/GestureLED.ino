/*
  Button LED

  This example creates a Bluetooth® Low Energy peripheral with service that contains a
  characteristic to control an LED and another characteristic that
  represents the state of the button.

  The circuit:
  - Arduino MKR WiFi 1010, Arduino Uno WiFi Rev2 board, Arduino Nano 33 IoT,
    Arduino Nano 33 BLE, or Arduino Nano 33 BLE Sense board.
  - Button connected to pin 4

  You can use a generic Bluetooth® Low Energy central app, like LightBlue (iOS and Android) or
  nRF Connect (Android), to interact with the services and characteristics
  created in this sketch.

  This example code is in the public domain.
*/

#include <ArduinoBLE.h>
#include <Arduino_APDS9960.h>

const int ledPin = LED_BUILTIN; // set ledPin to on-board LED
//const int buttonPin = 4; // set buttonPin to digital pin 4

BLEService ledService("19B10010-E8F2-537E-4F6C-D104768A1214"); // create service

// create switch characteristic and allow remote device to read and write
BLEByteCharacteristic ledCharacteristic("19B10011-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite);
// create button characteristic and allow remote device to get notifications
BLECharacteristic gestureCharacteristic("19B10012-E8F2-537E-4F6C-D104768A1214", BLERead | BLENotify, "00000000000000000000000000000000");

void setup() {
  Serial.begin(9600);

  pinMode(ledPin, OUTPUT); // use the LED as an output
  //pinMode(buttonPin, INPUT); // use button pin as an input

  if (!APDS.begin()) {
    Serial.println("* Error initializing APDS9960 sensor!");
  }
    APDS.setGestureSensitivity(80); 

  // begin initialization
  if (!BLE.begin()) {
    Serial.println("starting Bluetooth® Low Energy module failed!");

    while (1);
  }

  // set the local name peripheral advertises
  BLE.setLocalName("BLE:GestureLED3");
  // set the UUID for the service this peripheral advertises:
  BLE.setAdvertisedService(ledService);

  // add the characteristics to the service
  ledService.addCharacteristic(ledCharacteristic);
  ledService.addCharacteristic(gestureCharacteristic);

  // add the service
  BLE.addService(ledService);

  ledCharacteristic.writeValue(0);
  gestureCharacteristic.writeValue("0");

  // start advertising
  BLE.advertise();

  Serial.println("Bluetooth® device active, waiting for connections...");
}

int gesture = -1;
int oldGestureValue = -1;

void loop() {
  // poll for Bluetooth® Low Energy events
  BLE.poll();

  // read the current button pin state
  //char buttonValue = digitalRead(buttonPin);

    gesture = gestureDetectection();
    bool gestureChanged = (oldGestureValue != gesture);


  // has the value changed since the last read
  //bool buttonChanged = (buttonCharacteristic.value() != buttonValue);

  if (gestureChanged) {
    // button state changed, update characteristics
    ledCharacteristic.writeValue(gesture);
    if(gesture != 0) {

      //gesture += 10;

      //char gestureOut[32];
      //itoa(gesture, gestureOut, 10);
      char* gestureOut;

      if(gesture==1) gestureOut = "right";
      else if(gesture==2) gestureOut = "down";
      else if(gesture==3) gestureOut = "up";
      else gestureOut = "INVALID";

      Serial.print(gestureOut);
      gestureCharacteristic.writeValue(gestureOut);
    }
    else
      gestureCharacteristic.writeValue("left");
  }

  if (ledCharacteristic.written() || gestureChanged) {
    // update LED, either central has written to characteristic or button state has changed
    if (ledCharacteristic.value()) {
      Serial.println("LED on");
      digitalWrite(ledPin, HIGH);
    } else {
      Serial.println("LED off");
      digitalWrite(ledPin, LOW);
    }
  }
}

int gestureDetectection() {
  if (APDS.gestureAvailable()) {
    gesture = APDS.readGesture();

    switch (gesture) {
      case GESTURE_UP:
        Serial.println("- UP gesture detected");
        break;
      case GESTURE_DOWN:
        Serial.println("- DOWN gesture detected");
        break;
      case GESTURE_LEFT:
        Serial.println("- LEFT gesture detected");
        break;
      case GESTURE_RIGHT:
        Serial.println("- RIGHT gesture detected");
        break;
      default:
        Serial.println("- No gesture detected");
        break;
      }
    }
    return gesture;
}
