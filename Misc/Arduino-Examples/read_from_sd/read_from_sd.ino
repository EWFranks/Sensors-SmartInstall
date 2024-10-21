
#include <Arduino_MKRIoTCarrier.h>
#include <ArduinoJson.h>

#include "config.h"


struct Config {

  char device_id[8]; // Should be 4 character id, but adding 8 to accomodate later naming schemes
  char mqtt_ip[16];
  int mqtt_port;

  char server_ip[16];
  int server_port;
  char update_file[32];

  bool carrier_enabled;
  pin_size_t pir_pin;

};


MKRIoTCarrier carrier;




void setup() {
  // put your setup code here, to run once:

  
  Serial.begin(9600);
  delay(2000);
  Serial.println("Ready:");

  
  Config globalConfig;

  carrierSetup();
  String filename = "c.txt";

  Serial.println(SD.exists(filename));

  if(SD.exists(filename))
    loadConfiguration(filename, globalConfig);
  else
    saveConfiguration(filename, globalConfig);

  printConfigStruct(globalConfig);
  printFile(filename);
    

}

void loop() {
}

void saveConfiguration(String filename, const Config &globalConfig) { //ALTER

  Serial.println("Saving configuration to File:");

  File configFile = SD.open(filename, FILE_WRITE);

  if(!configFile) {
    Serial.println("Failed to create config file, GRRRRRRRRRR");
    return;
  }

  StaticJsonDocument<2048> configDoc;


  configDoc["device_id"] = DEVICE_ID;
  configDoc["mqtt_ip"] = MQTT_IP;
  configDoc["mqtt_port"] = MQTT_PORT;

  configDoc["server_ip"] = SERVER_IP;
  configDoc["server_port"] = SERVER_PORT;
  configDoc["update_file"] = UPDATE_FILE;

  configDoc["carrier_enabled"] = CARRIER_ENABLED;
  configDoc["pir_pin"] = PIR_PIN;


  if(serializeJson(configDoc, configFile) == 0) {
    Serial.println("Failed! haha! WOOOOOOH yes good. FAILURE haha");
  }

  configFile.close();
}

void loadConfiguration(String filename, Config &globalConfig) {

  Serial.println("Loading configuration:");

  File configFile = SD.open(filename, FILE_READ);
  StaticJsonDocument<2048> configDoc;


  DeserializationError error = deserializeJson(configDoc, configFile);

  if(error) {
    Serial.println("Failed to read file, GRRRRRRRRRRR");
    return;
  }


  strlcpy(globalConfig.device_id, configDoc["device_id"], sizeof(globalConfig.device_id));
  
  strlcpy(globalConfig.mqtt_ip, configDoc["mqtt_ip"], sizeof(globalConfig.mqtt_ip));
  globalConfig.mqtt_port = configDoc["mqtt_port"];

  strlcpy(globalConfig.server_ip, configDoc["server_ip"], sizeof(globalConfig.server_ip));
  globalConfig.server_port = configDoc["server_port"];
  strlcpy(globalConfig.update_file, configDoc["update_file"], sizeof(globalConfig.update_file));

  globalConfig.carrier_enabled = configDoc["carrier_enabled"];
  globalConfig.pir_pin = configDoc["pir_pin"];
}


void printConfigStruct(const Config &globalConfig) {
  
  Serial.println("Printing config struct:");
  Serial.println("Device ID: " + String(globalConfig.device_id));
  Serial.println("MQTT IP: " + String(globalConfig.mqtt_ip));
  Serial.println("MQTT Port: " + String(globalConfig.mqtt_port));
  Serial.println("Server IP: " + String(globalConfig.server_ip));
  Serial.println("Server Port: " + String(globalConfig.server_port));
  Serial.println("Update File: " + String(globalConfig.update_file));
  Serial.println("Carrier Enabled: " + String(globalConfig.carrier_enabled));
  Serial.println("PIR Pin: " + String(globalConfig.pir_pin));
}


void printFile(String filename) {
  File configFile = SD.open(filename, FILE_READ);
  StaticJsonDocument<2048> configDoc;

  DeserializationError error = deserializeJson(configDoc, configFile);

  if(error) {
    Serial.println("Failed to read file, GRRRRRRRRRRR");
    return;
  }

  Serial.println("Printing file contents:");

  Serial.println("Device ID: " + String(configDoc["device_id"].as<const char*>()));
  Serial.println("MQTT IP: " + String(configDoc["mqtt_ip"].as<const char*>()));
  Serial.println("MQTT Port: " + String(configDoc["mqtt_port"].as<int>()));
  Serial.println("Server IP: " + String(configDoc["server_ip"].as<const char*>()));
  Serial.println("Server Port: " + String(configDoc["server_port"].as<int>()));
  Serial.println("Update File: " + String(configDoc["update_file"].as<const char*>()));
  Serial.println("Carrier Enabled: " + String(configDoc["carrier_enabled"].as<bool>()));
  Serial.println("PIR Pin: " + String(configDoc["pir_pin"].as<int>()));

  

}



/**
 * @brief Initializes the necessary settings for the MKR IoT Carrier Rev2.
 * 
 */
void carrierSetup() {

  Serial.println("Carrier Setting Up:");

  carrier.noCase();

  Serial.println("Carrier has no case!");

  if(!carrier.begin()) {
    Serial.println("Carrier not connected, check connections");
  }

  Serial.println("Carrier has begun!");

  carrier.Light.begin();
  carrier.Light.setGestureSensitivity(100);

  Serial.println("Carrier done!");

}
