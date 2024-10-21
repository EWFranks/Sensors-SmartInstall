
#include <ArduinoMqttClient.h>
#include <Arduino_MKRIoTCarrier.h>
#include <WiFiNINA.h>

#include "arduino_secrets.h"
#include "config.h"

  // Enter broker IP in Config
char ipStr[] = BROKER_IP;

  // Enter credentials in arduino_secrets.h
char ssid[] = SECRET_SSID;
char pass[] = SECRET_PASS;


String deviceID;

int status = WL_IDLE_STATUS; // Wifi radio's status

WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);

IPAddress brokerIP;
int port = 1883;


  // MQTT VARIABLES
//Interval for sending messages in milliseconds
const long interval = 4000;
unsigned long previousMillis = 0;

int cursorX = 30;
int cursorY;

MKRIoTCarrier carrier;




void networkConnect() {
  // Connect to WiFi

  while(status != WL_CONNECTED) { // While not connected
      Serial.println("Attempting to connect to network: ");
      Serial.println(ssid);

      ScreenPrint("Connecting: " + String(ssid));

      // FOR WPA2 CONNECTION:
      status = WiFi.begin(ssid, pass); // Connect to WPA/WPA2 network
      delay(15000); // Wait 15 seconds for connection
    }

// When connected:  
  Serial.println("You're connected to the network! Yay!");
  IPAddress ip = WiFi.localIP();
  Serial.println(ip);

}


void brokerConnect() {
  // Connect to MQTT Broker

  brokerIP.fromString(ipStr);

  Serial.print("Attempting to connect to the MQTT broker: ");
  Serial.println(brokerIP);

  ScreenPrint("Connecting: " + String(brokerIP));
  
  while(!mqttClient.connect(brokerIP, port)) {
    Serial.print("MQTT connection failed! Error code = ");
    Serial.println(mqttClient.connectError());
    networkConnect();
   }

    Serial.println("You're connected to the MQTT broker!");
    Serial.println();
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



void setup() {
  // Init function, runs before anything else

  carrier.noCase();
  randomSeed(analogRead(0));

//Initialize serial communication and wait for port to open:
  Serial.begin(9600); // Attempt to connect to serial communication
  //while(!Serial); // Stall if not connected

  if(!carrier.begin()) {
    Serial.println("Carrier not connected, check connections");
    while(1);
  }

  carrier.Light.begin();
  carrier.Light.setGestureSensitivity(100);

  networkConnect(); // Connect to Wifi
  brokerConnect(); // Connect to MQTT broker

  setID();


}


// --------------------------------------- LOOP FUNCTIONS ---------------------------------------------



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



void SendMessage(String topic, String value, String unit) {
  // Send message through MQTT, and print message to both serial (if available) and to carrier screen

// Print to serial
  Serial.print("Sending message to topic: ");
  Serial.println(topic);
  Serial.println(value);

// Print to carrier
  carrier.display.setCursor(cursorX, cursorY);  // Set text horizontal (static) and vertical position
  //carrier.display.setTextColor(ST77XX_WHITE); // Color already set
  carrier.display.print(topic + ": " + value + " " + unit);
  cursorY += 10;  // Change text vertical position for next line

// Send through MQTT
  mqttClient.beginMessage(topic + "/" + deviceID);
  mqttClient.print(value + " " + unit);
  mqttClient.endMessage();

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

void MqttHandler() {
  // Handles the retrieval and sending of sensor readings w/ MQTT
  // Runs every X seconds


    DisplayHandler(); // Clear screen and write device ID
    

// Get environment values
    float temp = carrier.Env.readTemperature();
    int humidity = carrier.Env.readHumidity();
    int pressure = carrier.Pressure.readPressure();
    

// Send readings    
    SendMessage("temperature", String( temp ), "celsius");
    SendMessage("humidity", String( humidity ), "percent");
    SendMessage("pressure", String( pressure ), "hectopascal");

    Serial.println();

}


void loop() {

  // call poll() regularly to allow the library to send MQTT keep alive which avoids being disconnected by the broker
  mqttClient.poll();

  ConnectionCheck(); // Check Network and MQTT connection

// SENDING INTERVAL
  unsigned long currentMillis = millis();

  if(currentMillis - previousMillis >= interval) {  // If it is time to read sensor data again
    previousMillis = currentMillis; // Saves last time message was sent

    MqttHandler();

  } //else processingHandler();   // If and/or when we are waiting for the next MQTT interval, do NOT run idle, rather, do work.
  


}

