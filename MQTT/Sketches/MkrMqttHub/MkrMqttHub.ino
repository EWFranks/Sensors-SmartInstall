
#include <WiFiNINA.h>
#include <ArduinoJson.h>

// Include secrets and config initialization file
#include "sketch_secrets.h"
#include "init_config.h"

#include "src\libraries\ArduinoMqttClient\ArduinoMqttClient.h"

// Additional cpp logic
  #include "src\headers\SdConfig.h"
  #include "src\headers\MkrCarrierHelper.h"
  #include "src\headers\SensorPinHelper.h"
  #include "src\headers\CommandHandler.h"
  #include "src\headers\OtaUpdate.h"

  Config config;
  MkrCarrierHelper carrierHelper;
  SensorPinHelper sensorPinHelper;
  CommandHandler commandHandler;


// Connection Variables
  unsigned long previousMillis = 0;

  WiFiClient wifiClient; // Create an ESP8266 WiFiClient class to connect to the MQTT server
  MqttClient mqttClient(wifiClient);




void setup() {

    Serial.begin(9600); // Initialize serial port
    delay(2000);

    initExtendedLogic(); // Inits carrier wrapper and config objects
    
    // Checks for a new version of this sketch, and downloads it if it exists
    if(wifiConnect()) // Attempt a connect to WiFi for update
      checkUpdate(wifiClient, &config, SERVER_IP, SERVER_PORT); // Functionality within OtaUpdate file


    serverConnection(); // Connect to the WiFi (if not already) and MQTT broker

    // Everything connected, begin the loop
    quickPublish("data/config/" + String(config.options.device_id), config.toString()); // Publish config after connection
    carrierHelper.jinglePositive(); // Play a positive jingle

}


void loop() {

    mqttClient.poll(); // Poll the MQTT client
    serverConnection(); // Ensure connection to MQTT Broker

    eventLoop(); // Check for events not synced with the MQTT interval

    if(intervalCheck()) { // If the interval has passed since the last reading
      previousMillis = millis(); // Reset the interval timer
        
      sendingHandler(); // Send MQTT messages
    } 
}



// ====================== Loop Logic ======================


/**
 * @brief Handles the sending of sensor readings through MQTT messages.
 * 
 */
void sendingHandler() {

  Serial.println("Starting MQTT Handler:");
  carrierHelper.screenHandler(config.options.device_id); // Get the screen ready for displaying readings

  // Create a JSON document w/ a readings array
  StaticJsonDocument<2048> mqttDoc;
  JsonArray readings = mqttDoc.createNestedArray("readings");

  // Insert readings into readings array
  carrierHelper.addCarrierReadings(readings, &config);
  sensorPinHelper.addPinReadings(readings);

  if(readings.size() > 0) // If there are actual readings to send
    sendJsonMqtt(mqttDoc); // Send the JSON to the MQTT broker
}


/**
 * @brief Serialize the given JsonDocument into a JSON string and send it as a message to the MQTT broker.
 * 
 * @param mqttDoc The JsonDocument to be serialized and sent.
 */
void sendJsonMqtt(const JsonDocument& mqttDoc) {
  char out[1024];
  int b = serializeJson(mqttDoc, out); // Create a JSON string from the JSON object

  Serial.print("bytes = ");
  Serial.println(b, DEC);
  Serial.println(out);

  // Print to MQTT
  String topicString = "readings/" + String(config.options.device_id);

  mqttClient.beginMessage(topicString, false, 1, false); // arguments: (topic, retain flag, qos value, dup flag)
  mqttClient.print(out);
  mqttClient.endMessage();

  Serial.println();

}


/**
 * @brief Handles events that trigger outside of the MQTT sending interval.
 * 
 */
void eventLoop() {
  carrierHelper.accelerationCarrierAvailable(); // Check for acceleration events
  sensorPinHelper.loop(); // Check for events outside of sending interval
}



// ====================== Connection Logic ======================

// WiFi Variables
  int wifiStatus = WL_IDLE_STATUS; // Wifi radio's status
  int maxConnectionAttempts = 5; // Maximum number of connection attempts


/**
 * @brief Connects to both the WiFi and MQTT broker.
 * 
 */
void serverConnection() {
  int connectionAttempts = 0;

    while(!mqttClient.connected()) { // While not connected to the MQTT broker
        if(connectionAttempts >= maxConnectionAttempts)
          maxConnectionAttemptsReached(); // Restart the board if the max connection attempts have been reached
 
        connectionAttempts += 1;

        if(wifiConnect()) // Connect to WiFi. If successful:
            mqttConnect(BROKER_IP, BROKER_PORT); // Connect to the MQTT broker 
    }
}


/**
 * @brief Connects to the WiFi network.
 * 
 */
bool wifiConnect() {

    if(wifiStatus != WL_CONNECTED) {
        if(SECRET_USER != "") { // If there is a username
            wifiStatus = WiFi.beginEnterprise(SECRET_SSID, SECRET_USER, SECRET_PASS); // Connect to WPA2 Enterprise network.
            Serial.println("DEBUG: Attempting to connect to network: " + String(SECRET_SSID) + " with username: " + String(SECRET_USER) + " and password: " + String(SECRET_PASS));
        }
        else // Else, no username
            wifiStatus = WiFi.begin(SECRET_SSID, SECRET_PASS); // Connect to common WPA/WPA2 network.

        Serial.println("Attempting to connect to network: " + String(SECRET_SSID));
        carrierHelper.screenSet("Connecting WiFi: " + String(SECRET_SSID)); // Print Network Connection to Carrier Screen
        delay(15000); // Wait 15 seconds for connection
    }

    if(wifiStatus == WL_CONNECTED) {
        Serial.print("Connected to network. IP: ");
        Serial.println(WiFi.localIP());

        return true;
    }

    Serial.println("WiFi connection failed."); // Else, connection failed
    return false;
}


/**
 * @brief Connects to the MQTT broker, and subscribes to the needed topics.
 * 
 * @param ipStr The IP address of the MQTT broker.
 * @param port The port of the MQTT broker.
 */
bool mqttConnect(char ipStr[], int port) {
    IPAddress brokerIP;
    brokerIP.fromString(ipStr);
    
    Serial.println("Attempting to connect to broker: " + String(ipStr));
    carrierHelper.screenSet("Connecting MQTT: " + String(ipStr)); // Print MQTT Connection to Carrier Screen

    mqttClient.setUsernamePassword(MQTT_USER, MQTT_PASS);

    if(!mqttClient.connect(brokerIP, port)) { // Connect to the MQTT broker
        Serial.println("MQTT connection failed.");
        return false;
    }

    // Command setup
    mqttClient.onMessage(onMessage); // Set callback function for receiving messages
    mqttClient.subscribe("commands/all_/#"); // Subscribe to commands for all devices
    mqttClient.subscribe("commands/" + String(config.options.device_id) + "/#"); // Subscribe to device-specific commands

    Serial.println("You're connected to the MQTT broker!");
    return true;
}


/**
 * @brief Callback function for receiving messages.
 * 
 * @param messageSize The size of the message in bytes.
 */
void onMessage(int messageSize) {
  String messageTopic = mqttClient.messageTopic();
  String message = "";

  // Get the message from the MQTT broker
  while (mqttClient.available())
    message += (char)mqttClient.read(); 
  
  commandHandler.handleCommand(messageTopic, message); // Send command off to commandHandler
}


/**
 * @brief Restarts the board if the board cannot connect to the WiFi and/or the MQTT broker.
 * 
 */
void maxConnectionAttemptsReached() {
  Serial.println("Max connection attempts reached. Restarting...");
  carrierHelper.jingleNegative();
  
  NVIC_SystemReset();
}



// ====================== Extended Logic ======================


/**
 * @brief Initializes the custom helper classes.
 * 
 */
void initExtendedLogic() {
  // Init custom helper classes: carrier wrapper, config, command handler
  carrierHelper.carrierSetup();
  beginConfig();

  sensorPinHelper.initPointers(&config, &carrierHelper); // Initialize the sensor pin helper class
  commandHandler.initPointers(&config, &carrierHelper); // Initialize the command handler class

}
  

/**
 * @brief Begins the config object and loads the options from the SD card.
 * If the options cannot be loaded, the default options are used and saved to the SD card.
 * 
 */
void beginConfig() {
  config.begin("config.txt");

  if(!config.loadOptions()) {
    Serial.println("Options cannot be loaded! Creating new options!");

    // Copy the strings into the char arrays within the options construct
    strlcpy(config.options.device_id, DEVICE_ID, sizeof(config.options.device_id));
    strlcpy(config.options.update_file, UPDATE_FILE, sizeof(config.options.update_file));
    config.options.mqtt_interval = MQTT_INTERVAL; // Set the MQTT interval integer

    config.saveOptions(); // Save the options to the SD card

  } else Serial.println("Options loaded!");

}



// ====================== Misc Logic ======================


/**
 * @brief Checks to see if the sensor data should be read and sent to the MQTT broker.
 * 
 * @return true if the interval has passed since the last reading.
 * @return false otherwise.
 * 
 */
bool intervalCheck() {
    return millis() - previousMillis >= config.options.mqtt_interval; // May need to not directly reference config later
}


/**
 * @brief Publishes a message to the MQTT broker, with the given topic and message.
 * 
 * @param topic The topic to publish to.
 * @param message The message to publish.
 * 
 */
void quickPublish(String topic, String message) {
  Serial.println("Publishing to topic: " + topic + " with message: " + message);
  
  // Publish message to MQTT on topic 
  mqttClient.beginMessage(topic, false, 1, false); // topic, retain flag, qos value, dup flag
  mqttClient.print(message);
  mqttClient.endMessage();
}