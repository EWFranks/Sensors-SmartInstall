#include <WiFiNINA.h>
#include <Arduino_MKRIoTCarrier.h>
#include <ArduinoJson.h>

#include "src\ArduinoMqttClient\ArduinoMqttClient.h" // Modified version of ArduinoMqttClient

// Included DEFINES
  #include "arduino_secrets.h"
  #include "config.h"


// Set either on SD or in Config
  String deviceID;
  int version = 0;
  String directory = "";

// WiFi Variables
  int status = WL_IDLE_STATUS; // Wifi radio's status
  WiFiClient wifiClient;

// MQTT Variables
  MqttClient mqttClient(wifiClient);

  const long interval = 4000; // Interval for sending messages in milliseconds
  int port = 1883; // MQTT port

  unsigned long currentMillis = 0;
  unsigned long previousMillis = 0;


// Carrier Variables
  int cursorX = 30;
  int cursorY;

  MKRIoTCarrier carrier;



// ------ MAIN FUNCTIONS --------


void setup() {

  //Initialize serial communication and wait for port to open:
  Serial.begin(9600); // Attempt to connect to serial communication
  
  carrierSetup(); // Setup Carrier Functions
  getMetadata(); // Read data from SD Card

  networkConnect(); // Connect to Wifi
  handleSketchDownload(); // Check for OTA update

  brokerConnect(); // Connect to MQTT broker

  jinglePositive(); // Play restart sound

}



void loop() {

  mqttClient.poll(); // Check if new messages have arrived, keep MQTT connection alive
  ConnectionCheck(); // Check Network and MQTT connection

  //carrierAvailablity(); // async detect color/motion/acceleration
  loopCalculations();

  if(intervalCheck()) {  // If it is time to read sensor data again
    previousMillis = currentMillis; // Saves last time message was sent

    MqttHandler();

  }
}




// --------- SETUP FUNCTIONS --------

/**
 * @brief Initializes the necessary settings for the MKR IoT Carrier Rev2.
 * 
 */
void carrierSetup() {

  carrier.noCase();

  if(!carrier.begin()) {
    Serial.println("Carrier not connected, check connections");
    while(1);
  }

  carrier.Light.begin();
  carrier.Light.setGestureSensitivity(100);
}



// Enter credentials in arduino_secrets.h
  char ssid[] = SECRET_SSID;
  char pass[] = SECRET_PASS;

/**
 * @brief Connects to the configured network.
 * 
 */
void networkConnect() {
  // Connect to WiFi

  while(status != WL_CONNECTED) { // While not connected
      Serial.println("Attempting to connect to network: ");
      Serial.println(ssid);

      ScreenPrint("Connecting: " + String(ssid)); // Print to carrier screen

      // FOR WPA2 CONNECTION:
      status = WiFi.begin(ssid, pass); // Connect to WPA/WPA2 network
      delay(15000); // Wait 5 seconds for connection
    }

  // When connected:  
  Serial.println("You're connected to the network! Yay!");
  IPAddress ip = WiFi.localIP();
  Serial.println(ip);

}



// Enter in Config
  char ipStr[] = BROKER_IP; 

/**
 * @brief Connects to the configured MQTT broker.
 * 
 */
void brokerConnect() {
  // Connect to MQTT Broker

  Serial.print("Attempting to connect to the MQTT broker: ");
  Serial.println(ipStr);


  IPAddress brokerIP;
  brokerIP.fromString(ipStr);
  
  while(!mqttClient.connect(brokerIP, port)) { // While not connected to the broker
    Serial.print("MQTT connection failed! Error code = ");
    Serial.println(mqttClient.connectError());

    ScreenPrint("Connecting: " + String(ipStr));
      
    networkConnect(); // Connect to WiFi

   }

   mqttClient.onMessage(onCommandMessage); // Callback function for receiving commands

   mqttClient.subscribe("commands/all_/#"); // Subscribe to commands for all boards
   mqttClient.subscribe("commands/" + deviceID + "/#"); // Subscribe to commands for this board

    Serial.println("You're connected to the MQTT broker!");
    Serial.println();
}


/**
 * @brief Prints the given message to the carrier screen.
 * 
 * @param message The message to be printed.
 */
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



// --------- LOOP FUNCTIONS --------

/**
 * @brief Handles the retrieval and sending of sensor readings w/ MQTT.
 * Runs every X seconds.
 * 
 */
void MqttHandler() {

  DisplayHandler(); // Clear screen and write device ID

  // Send readings
  StaticJsonDocument<1024> mqttDoc;
  JsonArray readings = mqttDoc.createNestedArray("readings");

  readCarrier(readings);


  char out[1024];
  int b = serializeJson(mqttDoc, out); // JSON to String for sending

  // Print to Serial
  Serial.print("bytes = ");
  Serial.println(b, DEC);
  Serial.println(out);

  // Print to MQTT
  String topicString = "readings/" + deviceID;

  mqttClient.beginMessage(topicString, false, 1, false); // arguments: (topic, retain flag, qos value, dup flag)
  mqttClient.print(out);
  mqttClient.endMessage();

  Serial.println();

}

/**
 * @brief Maintains connection to WiFi and MQTT broker.
 * Called every loop.
 * 
 */
void ConnectionCheck() {

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

/**
 * @brief Handles the display every loop.
 * Clears the screen, set text settings, and write device ID.
 * 
 */
void DisplayHandler() {

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


bool intervalCheck() {
  currentMillis = millis();
  return currentMillis - previousMillis >= interval;
}
