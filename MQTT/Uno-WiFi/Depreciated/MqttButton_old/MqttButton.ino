#include <WiFiNINA.h>
#include <ArduinoJson.h>

//#include <ArduinoMqttClient.h>
#include "src\ArduinoMqttClient\ArduinoMqttClient.h"


// Included DEFINES
  #include "arduino_secrets.h"
  #include "config.h"

// Enter in Config
  char ipStr[] = BROKER_IP; 
  String deviceID = DEVICE_ID;

// Enter credentials in arduino_secrets.h
  char ssid[] = SECRET_SSID;
  char pass[] = SECRET_PASS;


// WiFi Variables
  int status = WL_IDLE_STATUS; // Wifi radio's status

  WiFiClient wifiClient;
  MqttClient mqttClient(wifiClient);


// MQTT Variables
  const long interval = 30000; // Interval for sending messages in milliseconds
  int port = 1883; // MQTT port

  unsigned long currentMillis = 0;
  unsigned long previousMillis = 0;


// ------ MAIN FUNCTIONS --------


void setup() {
  // Init function, runs before anything else

  //Initialize serial communication and wait for port to open:
  Serial.begin(9600); // Attempt to connect to serial communication

  networkConnect(); // Connect to Wifi
  brokerConnect(); // Connect to MQTT 
  
  MqttHandler();

}



void loop() {

  // call poll() regularly to allow the library to send MQTT keep alive
  // and to check if new messages have arrived (if applicable)
  mqttClient.poll();

  ConnectionCheck(); // Check Network and MQTT connection

  buttonHandler();


  if(intervalCheck()) {  // If it is time to read sensor data again
    previousMillis = currentMillis; // Saves last time message was sent

    MqttHandler();

  }
}




// --------- SETUP FUNCTIONS --------


void networkConnect() {
  // Connect to WiFi

  while(status != WL_CONNECTED) { // While not connected
      Serial.println("Attempting to connect to network: ");
      Serial.println(ssid);

      // FOR WPA2 CONNECTION:
      status = WiFi.begin(ssid, pass); // Connect to WPA/WPA2 network
      delay(5000); // Wait 5 seconds for connection
    }

  // When connected:  
  Serial.println("You're connected to the network! Yay!");
  IPAddress ip = WiFi.localIP();
  Serial.println(ip);

}



void brokerConnect() {
  // Connect to MQTT Broker

  Serial.print("Attempting to connect to the MQTT broker: ");
  Serial.println(ipStr);


  IPAddress brokerIP;
  brokerIP.fromString(ipStr);

  mqttClient.setUsernamePassword(MQTT_USER, MQTT_PASS);
  
  while(!mqttClient.connect(brokerIP, port)) { // While not connected to the broker
    Serial.print("MQTT connection failed! Error code = ");
    Serial.println(mqttClient.connectError());
      
    networkConnect(); // Connect to WiFi

   }

   mqttClient.onMessage(onCommandMessage);

   mqttClient.subscribe("commands/all_/#");
   mqttClient.subscribe("commands/" + deviceID + "/#");

    Serial.println("You're connected to the MQTT broker!");
    Serial.println();
}



// --------- LOOP FUNCTIONS --------


void addReading(JsonArray readings, String readingType, String readingVal, String readingUnit = "") {

  JsonObject readingObject = readings.createNestedObject();

  readingObject["reading_type"] = readingType;
  if(readingUnit != "")
    readingObject["reading_unit"] = readingUnit;
  
  readingObject["reading_val"] = readingVal;

}

void sendReading(String readingType, String readingVal, String readingUnit = "") {
  StaticJsonDocument<1024> mqttDoc;
  JsonArray readings = mqttDoc.createNestedArray("readings");


  addReading(readings, readingType, readingVal, readingUnit);


  char out[1024];
  int b = serializeJson(mqttDoc, out); // JSON to String for sending

  // Print to Serial
  Serial.print("bytes = ");
  Serial.println(b, DEC);
  Serial.println(out);

  // Print to MQTT
  mqttClient.beginMessage("readings/" + deviceID);
  mqttClient.print(out);
  mqttClient.endMessage();

  Serial.println();
}


void MqttHandler() {
  // Handles the retrieval and sending of sensor readings w/ MQTT
  // Runs every X seconds


  // Send readings
  sendReading("heartbeat", "N/A", "N/A");


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


bool intervalCheck() {
  currentMillis = millis();
  return currentMillis - previousMillis >= interval;
}


#define BUTTON_PIN A0

bool curPressed = false;

int buttonHandler() {

  int buttonVal = analogRead(BUTTON_PIN);

  if(buttonVal < 100 && !curPressed) {
    curPressed = true;
    sendReading("button", "true", "flag");

  } else if(buttonVal >= 100 && curPressed) {
    curPressed = false;
  }


}
