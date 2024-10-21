// This version records motion events, showing motion when there are 10 events in a quarter second period
// Author: Daniel Webster


#include <Arduino_MKRIoTCarrier.h>
#include <WiFiNINA.h>
#include "arduino_secrets.h"
#include "config.h"
#include <ArduinoJson.h>

//#include <ArduinoMqttClient.h>
#include "src\ArduinoMqttClient\ArduinoMqttClient.h"

#define pirPin A0


String deviceID;
char ipStr[] = BROKER_IP;

unsigned long currentMillis = 0;
unsigned long previousMillis = 0;
const long interval = 250;  // Changed interval to 1000 milliseconds (1 second)

char ssid[] = SECRET_SSID;
char pass[] = SECRET_PASS;

int status = WL_IDLE_STATUS;
WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);

IPAddress brokerIP;
int port = 1883;

MKRIoTCarrier carrier;


// Motion Variables
  int motion = -1;
  const int motionThreshold = 200; // Adjust this threshold based on your PIR sensor's sensitivity
  int previousMotion = 0;
  int cursorX = 30;
  int cursorY;

  int windowCount = 0;
  const int windowThreshold = 10;

  String topicMotion = "pir_motion";



void DisplayHandler() {
  // Clear screen, set text settings, and write device ID

  carrier.display.fillScreen(ST77XX_BLACK); // Clear screen

  cursorY = 80; // Reset text vertical position to close to top of the screen
  carrier.display.setCursor(cursorX, cursorY); // Set text horizontal and vertical position

  // Write ID
  carrier.display.setTextColor(0xF800); // Set text color to red
  carrier.display.setTextSize(2);
  carrier.display.print("Device ID: " + String(deviceID));

  carrier.display.setTextColor(ST77XX_WHITE); // Change text color to white for readings
  carrier.display.setTextSize(1); // Shrink reading text

  cursorY += 30;  // Change text vertical position for next line

}

void ScreenPrint(String message) {
  carrier.display.fillScreen(ST77XX_BLACK); // Clear screen

  cursorY = 80; // Reset text vertical position to close to top of the screen
  carrier.display.setCursor(cursorX, cursorY); // Set text horizontal and vertical position

  // Write ID
  carrier.display.setTextColor(0xF800); // Set text color to red
  carrier.display.setTextSize(1);
  carrier.display.print(message);

  carrier.display.setTextColor(ST77XX_WHITE); // Change text color to white for readings
  carrier.display.setTextSize(1); // Shrink reading text

}


void setup() {
  CARRIER_CASE = false;

  Serial.begin(9600);

  if (!carrier.begin()) {
    Serial.println("Carrier not connected, check connections");
    while (1);
  }

  networkConnect();
  brokerConnect();

  setID();

}


void loop() {
  
  mqttClient.poll();

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi connection lost. Retrying...");
    networkConnect();
    brokerConnect();
  }

  if (!mqttClient.connected()) {
    Serial.println("MQTT connection lost. Retrying...");
    brokerConnect();
  }

  unsigned long currentMillis = millis();
  int motion = analogRead(pirPin);  // Changed digitalRead to analogRead for PIR sensor

  if (motion > motionThreshold) {
    //Increase the noted motion events
    windowCount++;
    carrier.leds.fill(carrier.leds.Color(0, 200, 0), 0, 5);
    carrier.leds.show();
      
  }

  if (intervalCheck()) { 
    MqttHandler();
  }
  
}

bool intervalCheck() {
  currentMillis = millis();
  return currentMillis - previousMillis >= interval;
}

JsonObject addReading(JsonArray readings, String readingType, String readingVal, String readingUnit = "") {

  JsonObject readingObject = readings.createNestedObject();

  readingObject["reading_type"] = readingType;
  if(readingUnit != "")
    readingObject["reading_unit"] = readingUnit;
  
  readingObject["reading_val"] = readingVal;

  
  return readingObject;

}


void MqttHandler() {

    DisplayHandler();
      
    String motionString;

    StaticJsonDocument<1024> mqttDoc;
    JsonArray readings = mqttDoc.createNestedArray("readings");
    
    if (motion > motionThreshold) {
      if(previousMotion == 0 && windowCount >= windowThreshold){
        motionString = "Motion detected";

        //SendMessage(topicMotion, motionString, "");
        addReading(readings, topicMotion, motionString, "flag");


        carrier.leds.fill(carrier.leds.Color(0, 200, 0), 0, 5);
        previousMotion = 1;
      } 
    } else {
      if(previousMotion == 1){
        motionString = "No motion";
        carrier.leds.fill(carrier.leds.Color(0, 0, 200), 0, 5);
        
        //SendMessage(topicMotion, motionString, "");
        addReading(readings, topicMotion, motionString, "flag");

        previousMotion = 0;
      }
    }
    windowCount = 0; //reset the window count
    carrier.leds.show();

    char out[1024];
    int b = serializeJson(mqttDoc, out);

    Serial.print("bytes = ");
    Serial.println(b, DEC);
    Serial.println(out);

    mqttClient.beginMessage(deviceID);

}

// void SendMessage(char topic[], String value, char unit[]) {
//   Serial.print("Sending message to topic: ");
//   Serial.println(topic);
//   Serial.println(value);

//   carrier.display.setCursor(cursorX, cursorY);  // Set text horizontal (static) and vertical position
//   carrier.display.print(String(topic) + ": " + value + " " + String(unit));
//   cursorY += 10;  // Change text vertical position for next line

//   mqttClient.beginMessage(String(topic) + "/" + deviceID);
//   mqttClient.print(value + " " + String(unit));
//   mqttClient.endMessage();
// }

void networkConnect() {
  while (status != WL_CONNECTED) {
    Serial.println("Attempting to connect to network: ");
    Serial.println(ssid);

    ScreenPrint("Connecting: " + String(ssid));

    status = WiFi.begin(ssid, pass);
    delay(15000);
  }

  Serial.println("You're connected to the network! Yay!");
  IPAddress ip = WiFi.localIP();
  Serial.println(ip);
}



void brokerConnect() {
  brokerIP.fromString(ipStr);

  Serial.print("Attempting to connect to the MQTT broker: ");
  Serial.println(brokerIP);

  ScreenPrint("Connecting: " + String(ipStr));

  while (!mqttClient.connect(brokerIP, port)) {
    Serial.print("MQTT connection failed! Error code = ");
    Serial.println(mqttClient.connectError());
    networkConnect();
  }

  Serial.println("You're connected to the MQTT broker!");
  Serial.println();
}


void ConnectionCheck() {
  // Maintains conection to WiFi and MQTT
  // Called every loop

// Reconnection
  if(WiFi.status() != WL_CONNECTED) {   // If WiFi is disconnected, reconnect to WiFi and MQTT broker
    Serial.println("Wifi connection lost. Retrying...");

    networkConnect(); // Connect to WiFi
    brokerConnect();  // Connect to broker

  } else if(!mqttClient.connected()) {  // If broker is disconected (but not WiFi), reconnect to just MQTT broker  
    Serial.println("MQTT connection lost. Retrying...");

    brokerConnect();

  }
}




void setID() {
  // Find ID on SD card. If cannot find the ID, save the ID found in the config to the SD card for later

  if(SD.exists("ID.txt")) { // If file containing current ID exists on SD Card, read it
    Serial.println("Reading ID from SD Card...");

    File idFile = SD.open("id.txt", FILE_READ);
    while(idFile.available())
    deviceID += char(idFile.read());  // Get device ID from file
      
    idFile.close();


  } else { // If no file exists, create it and assign id from config.h
  
    Serial.println("Writing ID to SD Card...");

    File idFile = SD.open("id.txt", FILE_WRITE); // Create file

    Serial.println("Exists?: " + String(SD.exists("ID.txt")));

    if(idFile) {
      idFile.write(DEVICE_ID);  // Write config's id to file
      deviceID = DEVICE_ID;     // Set device id to config's id

      idFile.close();
    } else Serial.println("Unable to write ID to SD Card!");

  
  }

}



