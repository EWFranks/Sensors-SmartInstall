
#include <WiFiNINA.h>
#include <ArduinoJson.h>

#include <ArduinoBLE.h>


#include "src\ArduinoMqttClient\ArduinoMqttClient.h"


#include "arduino_secrets.h"
#include "config.h"


#include "utility/wifi_drv.h"

  // Enter broker IP in Config
char ipStr[] = BROKER_IP;

  // Enter credentials in arduino_secrets.h
char ssid[] = SECRET_SSID;
char pass[] = SECRET_PASS;

char ble_name[] = BLE_NAME;

String deviceID = DEVICE_ID;


int status = WL_IDLE_STATUS; // Wifi radio's status

WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);

IPAddress brokerIP;
int port = 1883; // MQTT port


  // MQTT VARIABLES
//Interval for sending messages in milliseconds
const long interval = 500;
unsigned long currentMillis = 0;
unsigned long previousMillis = 0;





void networkConnect() {
  // Connect to WiFi

  while(status != WL_CONNECTED) { // While not connected
      Serial.println("Attempting to connect to network: ");
      Serial.println(ssid);

      // FOR WPA2 CONNECTION:
      status = WiFi.begin(ssid, pass); // Connect to WPA/WPA2 network
      //delay(5000); // Wait 5 seconds for connection
    }

// When connected:  
  Serial.println("You're connected to the network! Yay!");
  IPAddress ip = WiFi.localIP();
  Serial.println(ip);

}



void brokerConnect() {
  // Connect to MQTT Broker

  brokerIP.fromString(ipStr);

  Serial.print(F("Attempting to connect to the MQTT broker: "));
  Serial.println(brokerIP);
  
  while(!mqttClient.connect(brokerIP, port)) {
    Serial.print(F("MQTT connection failed! Error code = "));
    Serial.println(mqttClient.connectError());
      
    networkConnect(); // Connect to WiFi

   }

    Serial.println(F("You're connected to the MQTT broker!"));
    Serial.println();
}

// --------- LOOP FUNCTIONS --------


JsonObject addReading(JsonArray readings, String deviceAddress, String deviceName, String rssiValue) {

  JsonObject readingObject = readings.createNestedObject();

  readingObject["address"] = deviceAddress;
  readingObject["name"] = deviceName;
  readingObject["rssi"] = rssiValue;
  
  return readingObject;

}



void ConnectionCheck() {
  // Maintains conection to WiFi and MQTT
  // Called every loop

  wifiClient = WiFiClient();
  mqttClient = MqttClient(wifiClient);

// Reconnection
  if(WiFi.status() != WL_CONNECTED) {   // If WiFi is disconnected, reconnect to WiFi and MQTT broker
    Serial.println(F("Wifi connection lost. Retrying..."));

    networkConnect(); // Connect to WiFi
    brokerConnect();  // Connect to broker

  } else if(!mqttClient.connected()) {  // If broker is disconected (but not WiFi), reconnect to just MQTT broker  
    Serial.println(F("MQTT connection lost. Retrying..."));

    brokerConnect();

  }
}





String knownAddresses[64];
String knownNames[64];
int knownRssi[64];

int idx = 0;

void MqttHandler() {
  // Handles the retrieval and sending of sensor readings w/ MQTT
  // Runs every X seconds

  BLE.end();
  status = WL_IDLE_STATUS;

  //networkConnect();

  //brokerConnect();

  wiFiDrv.wifiDriverDeinit();
  wiFiDrv.wifiDriverInit();

  ConnectionCheck();
  
  Serial.println("MQTT Handler Started!");

// Send readings


  StaticJsonDocument<512> mqttDoc;
  JsonArray rssiReadings = mqttDoc.createNestedArray("rssi");




    //addReading(readings, "temperature", String(temp), "celsius"); // Read temperature

  for(int i = 0; i < 64; i++) {
    Serial.print(knownAddresses[i] + " ");

    if(knownAddresses[i] != "") {
      addReading(rssiReadings, knownAddresses[i], knownNames[i], String( knownRssi[i] ));
    }

    knownAddresses[i] = "";
    knownNames[i] = "";
    knownRssi[i] = 0;

  }
      
  

  char out[512];
  int b = serializeJson(mqttDoc, out);
  Serial.print("bytes = ");
  Serial.println(b, DEC);
  Serial.println(out);

  mqttClient.beginMessage("rssi/" + deviceID);
  mqttClient.print(out);
  mqttClient.endMessage();

  Serial.println();

  mqttClient.poll();

  //knownAddresses = {};
  //knownNames = {};
  //knownRssi = {};

  Serial.println("MQTT Handler Ended!");

  //mqttClient.disconnect();
  //WiFi.end();

  delay(500);


  idx = 0;

  while(!BLE.begin()) {
    Serial.println(F("Bluetooth Low Energy failed!"));
  }

  //BLE.begin();
  BLE.setLocalName(ble_name);

  BLE.advertise();
  BLE.scan();

}



void scanHandler() {
    


    BLEDevice device = BLE.available();

    if(device) {

      Serial.println("Potential device I wonder?");

      String address = device.address();
      bool isUnique = true;

      for(int i = 0; i < 32; i++) {
        if(address.equals(knownAddresses[idx]))
          isUnique = false;

      }

      if(isUnique) {

        Serial.println(F("Device Found!"));
        Serial.println(device.address());
        //addReading(rssiReadings, device.address(), device.localName(), String( device.rssi() ));

        knownAddresses[idx] = device.address();
        knownRssi[idx] = device.rssi();
        
        if(device.hasLocalName())
          knownNames[idx] = device.localName();
        else
          knownNames[idx] = "";

        idx++;
      
      }

    } // After no more devices?
    



}


bool intervalCheck() {
  currentMillis = millis();
  return currentMillis - previousMillis >= interval;
}

// ------ MAIN FUNCTIONS --------



void setup() {
  // Init function, runs before anything else

//Initialize serial communication and wait for port to open:
  Serial.begin(9600); // Attempt to connect to serial communication

  //networkConnect(); // Connect to Wifi
  //brokerConnect(); // Connect to MQTT broker

  while(!BLE.begin()) {
    Serial.println(F("Bluetooth Low Energy failed! Trying again..."));
  }

  BLE.setLocalName(ble_name);
  BLE.advertise();

  Serial.println("Scanning:");
  BLE.scan();
  Serial.println("Scanning Complete!");



}


void loop() {

  // call poll() regularly to allow the library to send MQTT keep alive
  // and to check if new messages have arrived (if applicable)
  //Serial.println("Loop Start!");
  //mqttClient.poll();

  //Serial.println("Connection Check Start!");
  //ConnectionCheck(); // Check Network and MQTT connection

  scanHandler();


// SENDING INTERVAL

  //Serial.println("Interval check!");

  if(intervalCheck()) {  // If it is time to read sensor data again
    MqttHandler();
    previousMillis = millis(); // Saves last time message was sent

  }


}
