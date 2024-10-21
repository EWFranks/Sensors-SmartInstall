/**
 * @file SdConfig.cpp
 * @author Sterling Fisher (sjf2864@utulsa.edu)
 * 
 * @brief This library manages the configuration of the device, stored on the SD Card.
 * The Config object contains an Options struct, which holds all of the configuration options.
 * 
 * @version 0.1
 * @date 2023-10-25
 * 
 * @copyright Copyright (c) 2023
 * 
 */


#include "Arduino.h"

#include "SD.h"
#include "ArduinoJson.h"

#include "headers\EepromConfig.h"

/*

Needed Configuration Options:

device_id -> char array
mqtt_interval -> int

*/






Options options;
String _filename;

Config::Config() {}

void Config::begin() {
    initOptions();
}


/**
 * @brief Saves the data within the options struct to a config file on the SD card.
 * 
 */
bool Config::saveOptions() {

    Serial.println("Saving config to EEPROM:");

    // for(int idx = ID_ADDRESS; idx < sizeof(options.device_id); idx++) {
    //     EEPROM.write(idx, options.device_id[idx]);
    // }

    // EEPROM.write(INTERVAL_ADDRESS, options.mqtt_interval);

    EEPROM.put(ID_ADDRESS, options.device_id);
    EEPROM.put(INTERVAL_ADDRESS, options.mqtt_interval);

    return true;

}





/**
 * @brief Loads the data from the config file into the options struct.
 * 
 */
bool Config::loadOptions() {
    Serial.println("Loading config from EEPROM:");

    // for(int idx = ID_ADDRESS; idx < ID_ADDRESS + ID_LENGTH; idx++) {
    //     Serial.println("Reading from EEPROM: " + String(EEPROM.read(idx)));
    //     options.device_id[idx] = EEPROM.read(idx);
    // }

    EEPROM.get(ID_ADDRESS, options.device_id);
    EEPROM.get(INTERVAL_ADDRESS, options.mqtt_interval);

    return(options.mqtt_interval >= 0); // Checks to make sure that the interval is not invalid, which would indicate that the EEPROM has not been written to yet.
}



/**
 * @brief Initializes the options struct with default values, which can be changed in the base script.
 * 
 */
void Config::initOptions() {

    strlcpy(options.device_id, "AAAA", sizeof(options.device_id));
    options.mqtt_interval = 4000;

}



String Config::toString() {
    String optionsString = "";

    optionsString += "Device ID: " + String(options.device_id) + "\n";
    optionsString += "MQTT Interval: " + String(options.mqtt_interval) + "\n";

    return optionsString;
}