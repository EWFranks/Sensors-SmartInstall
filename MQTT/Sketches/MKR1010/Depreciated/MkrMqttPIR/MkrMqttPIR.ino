#include <WiFiNINA.h>
#include <Arduino_MKRIoTCarrier.h>
#include <ArduinoJson.h>

//#include <ArduinoMqttClient.h>
#include "src\ArduinoMqttClient\ArduinoMqttClient.h"


// Included DEFINES
  #include "arduino_secrets.h"
  #include "config.h"


// Enter in Config
  char ipStr[] = BROKER_IP; 

// Set either on SD or in Config
  String deviceID;
  int version = 0;
  String directory = "";

// Enter credentials in arduino_secrets.h
  char ssid[] = SECRET_SSID;
  char pass[] = SECRET_PASS;


// WiFi Variables
  int status = WL_IDLE_STATUS; // Wifi radio's status

  WiFiClient wifiClient;
  MqttClient mqttClient(wifiClient);


// MQTT Variables
  const long cooldown = 3000; // Cooldown for sending messages in milliseconds
  int port = 1883; // MQTT port

  unsigned long currentMillis = 0;
  unsigned long previousMillis = 0;


// Carrier Variables
  int cursorX = 30;
  int cursorY;

  MKRIoTCarrier carrier;

// PIR VARIABLES
  bool prevMotion = false;


// ------ MAIN FUNCTIONS --------


void setup() {
  // Init function, runs before anything else

  //Initialize serial communication and wait for port to open:
  Serial.begin(9600); // Attempt to connect to serial communication
  
  carrierSetup(); // Setup Carrier Functions
  getMetadata(); // Read data from SD Card

  networkConnect(); // Connect to Wifi
  handleSketchDownload(); // Check for OTA update

  brokerConnect(); // Connect to MQTT broker

  jinglePositive(); // Play restart sound

  carrier.leds.fill(carrier.leds.Color(0, 0, 200), 0, 5);
  carrier.leds.show();

}



void loop() {

  // call poll() regularly to allow the library to send MQTT keep alive
  // and to check if new messages have arrived (if applicable)
  mqttClient.poll();

  ConnectionCheck(); // Check Network and MQTT connection
  carrierAvailablity(); // async detect color/motion/acceleration

  if(cooldownCheck()) {

    if(eventCheck()) {
      previousMillis = currentMillis; // Saves last time message was sent for cooldown purposes
    
      MqttHandler();

    } else {

      prevMotion = false;
      carrier.leds.fill(carrier.leds.Color(0, 0, 200), 0, 5);
      carrier.leds.show();
    
    }

  }


}




// --------- SETUP FUNCTIONS --------


void carrierSetup() {
  // Initialize MKR IoT Carrier

  carrier.noCase();

  if(!carrier.begin()) {
    Serial.println("Carrier not connected, check connections");
    while(1);
  }

}



void networkConnect() {
  // Connect to WiFi

  while(status != WL_CONNECTED) { // While not connected
      Serial.println("Attempting to connect to network: ");
      Serial.println(ssid);

      ScreenPrint("Connecting: " + String(ssid)); // Print to carrier screen

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
  
  while(!mqttClient.connect(brokerIP, port)) { // While not connected to the broker
    Serial.print("MQTT connection failed! Error code = ");
    Serial.println(mqttClient.connectError());

    ScreenPrint("Connecting: " + String(ipStr));
      
    networkConnect(); // Connect to WiFi

   }

   mqttClient.onMessage(onCommandMessage);

   mqttClient.subscribe("commands/all_/#");
   mqttClient.subscribe("commands/" + deviceID + "/#");

    Serial.println("You're connected to the MQTT broker!");
    Serial.println();
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



// --------- LOOP FUNCTIONS --------

bool eventCheck() {
  int motion = analogRead(PIR_PIN);

  if(motion > MOTION_THRESHOLD && prevMotion == false) { // If motion is detected and motion hasn't already been detected during the cooldown
      carrier.leds.fill(carrier.leds.Color(0, 200, 0), 0, 5);
      carrier.leds.show();

      prevMotion = true;
      return true;
  }

  return false;
}


void MqttHandler() {
  // Handles the retrieval and sending of sensor readings w/ MQTT
  // Runs every X seconds

  DisplayHandler(); // Clear screen and write device ID

  // Send readings
  StaticJsonDocument<1024> mqttDoc;
  JsonArray readings = mqttDoc.createNestedArray("readings");

  //readCarrier(readings);
  // Build up JSON file

  addReading(readings, "pir_motion", "true", "flag");


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


bool cooldownCheck() {
  currentMillis = millis();
  return currentMillis - previousMillis >= cooldown;
}
