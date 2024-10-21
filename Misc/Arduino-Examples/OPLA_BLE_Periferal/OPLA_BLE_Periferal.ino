#include <Arduino_MKRIoTCarrier.h>
#include "visuals.h"
#include "pitches.h"
#include <ArduinoBLE.h>

MKRIoTCarrier carrier;

String state = "start";

//BLE additions


const char* deviceServiceUuid = "19b10000-e8f2-537e-4f6c-d104768a1214";
const char* deviceServiceCharacteristicUuid = "19b10001-e8f2-537e-4f6c-d104768a1214";

int gesture = -1;

BLEService gestureService(deviceServiceUuid); 
BLEByteCharacteristic gestureCharacteristic(deviceServiceCharacteristicUuid, BLERead | BLEWrite);
//End Additions

int count = 3;
uint32_t colorRed = carrier.leds.Color(200, 0, 0);
uint32_t colorGreen = carrier.leds.Color(0, 200, 0);
uint32_t colorBlue = carrier.leds.Color(0, 0, 200);

// notes in the melody:
int finalMelody[] = {
  NOTE_F5, NOTE_F5, NOTE_F5, NOTE_F5, NOTE_CS5, NOTE_DS5, NOTE_F5, NOTE_DS5, NOTE_F5
};

int celebrationMelody[] = {
  NOTE_F4, NOTE_F4, NOTE_F4, NOTE_F4, NOTE_CS4, NOTE_DS4, NOTE_F4, NOTE_DS4, NOTE_F4
};

// note durations: 4 = quarter note, 8 = eighth note, etc.:
int noteDurations[] = {
  12, 12, 12, 4, 4, 4, 6, 12, 1
};

//Declare pages
void configure();
void startPage();
void touchpage();
void sensorsPage();
void connectorsPage();
void actuatorsPage();

//Declare their flags
bool touchCheckCompleted = false;
bool shakeCheckCompleted = false;
bool colorCheckCompleted = false;
bool gestureCheckCompleted = false;
bool relay1CheckCompleted = false;
bool relay2CheckCompleted = false;


void setup()
{
  CARRIER_CASE = false;
  Serial.begin(9600);
  delay(1500);


  if (!carrier.begin())
  {
    Serial.println("Carrier not connected, check connections");
    while (1);
  }
  BLE.setLocalName("Arduino MKR IoT Carrier (Peripheral)");
  BLE.setAdvertisedService(gestureService);
  gestureService.addCharacteristic(gestureCharacteristic);
  BLE.addService(gestureService);
  gestureCharacteristic.writeValue(-1);
  BLE.advertise();
}

void writeGesture(int gesture) {
  Serial.println("- Characteristic <gesture_type> has changed!");
  
   switch (gesture) {
      case GESTURE_UP:
        //Serial.println("* Actual value: UP (Upper LED on)");
        //Serial.println(" ");
        carrier.leds.setPixelColor(2, colorBlue);
        break;
      case GESTURE_DOWN:
        //Serial.println("* Actual value: DOWN (lower LEDs on)");
        //.println(" ");
        carrier.leds.setPixelColor(0, colorBlue);
        carrier.leds.setPixelColor(4, colorBlue);
        break;
      case GESTURE_LEFT:
        //Serial.println("* Actual value: LEFT (Left LED on)");
        //Serial.println(" ");
        carrier.leds.setPixelColor(1, colorBlue);
        break;
      case GESTURE_RIGHT:
        //Serial.println("* Actual value: RIGHT (built-in LED on)");
        //Serial.println(" ");
        carrier.leds.setPixelColor(3, colorBlue);
        break;
      default:
        break;
    }      
}
void loop()
{

  BLEDevice central = BLE.central();
  //Serial.println("- Discovering central device...");
  delay(500);

  if (central) {
    //Serial.println("* Connected to central device!");
    //Serial.print("* Device MAC address: ");
    //Serial.println(central.address());
    //Serial.println(" ");

    while (central.connected()) {
      if (gestureCharacteristic.written()) {
         gesture = gestureCharacteristic.value();
         writeGesture(gesture);
       }
    }
  }
}
