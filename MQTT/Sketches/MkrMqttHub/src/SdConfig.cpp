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

#include "headers\SdConfig.h"

/*

Needed Configuration Options:

device_id
mqtt_ip, mqtt_port
server_ip, server_port, update_file

carrier_temperature, carrier_humidity, carrier_pressure, carrier_gas, carrier_VOC, carrier_CO2, carrier_acceleration -> bool
A6_Sensor, A0_Sensor, I2C_Sensor -> char arrays, Grove Sensors on Carrier

*/



Options options;
String _filename;

Config::Config() {}

void Config::begin(String filename) {
    
    _filename = filename;
    
    if(!SD.begin(0)) {
        Serial.println("SD Card failed to initialize, or is missing.");
        return;
    }

    initOptions();
}


/**
 * @brief Saves the data within the options struct to a config file on the SD card.
 * 
 */
bool Config::saveOptions() {
    Serial.println("Saving config to file: " + _filename);
    
    if(SD.exists(_filename)) {
        SD.remove(_filename);
    }

    File configFile = SD.open(_filename, FILE_WRITE); // TODO: Delete file if already exists

    if(!configFile) {
        Serial.println("Failed to open config file for writing");
        return false;
    }


    StaticJsonDocument<2048> configDoc;

    {
        configDoc["device_id"] = options.device_id;
        configDoc["version"] = options.version;

        configDoc["mqtt_interval"] = options.mqtt_interval;
        configDoc["update_file"] = options.update_file;
        
        configDoc["carrier_temperature"] = options.carrier_temperature;
        configDoc["carrier_humidity"] = options.carrier_humidity;
        configDoc["carrier_pressure"] = options.carrier_pressure;
        configDoc["carrier_gas"] = options.carrier_gas;
        configDoc["carrier_VOC"] = options.carrier_VOC;
        configDoc["carrier_CO2"] = options.carrier_CO2;
        configDoc["carrier_acceleration"] = options.carrier_acceleration;

        configDoc["A6"] = options.A6;
        configDoc["A0"] = options.A0;
        configDoc["I2C"] = options.I2C;
    }

    if (serializeJson(configDoc, configFile) == 0) {
        Serial.println("Failed to write to file");

        configFile.close();
        return false;
    }

    configFile.close();
    return true;

}


/**
 * @brief Loads the data from the config file into the options struct.
 * 
 */
bool Config::loadOptions() {
    Serial.println("Loading config from file: " + _filename);

    File configFile = SD.open(_filename, FILE_READ);

    if(!configFile) {
        Serial.println("Failed to open config file for reading.");
        return false;
    }

    StaticJsonDocument<2048> configDocument;

    DeserializationError error = deserializeJson(configDocument, configFile);

    if(error) {
        Serial.println("Failed to read config file.");

        configFile.close();
        return false;
    }

    // Loading options struct with file information
    
    {
        strlcpy(options.device_id, configDocument["device_id"], sizeof(options.device_id));
        options.version = configDocument["version"];

        options.mqtt_interval = configDocument["mqtt_interval"];
        strlcpy(options.update_file, configDocument["update_file"], sizeof(options.update_file));
        
        options.carrier_temperature = configDocument["carrier_temperature"];
        options.carrier_humidity = configDocument["carrier_humidity"];
        options.carrier_pressure = configDocument["carrier_pressure"];
        options.carrier_gas = configDocument["carrier_gas"];
        options.carrier_VOC = configDocument["carrier_VOC"];
        options.carrier_CO2 = configDocument["carrier_CO2"];
        options.carrier_acceleration = configDocument["carrier_acceleration"];

        strlcpy(options.A6, configDocument["A6"], sizeof(options.A6));
        strlcpy(options.A0, configDocument["A0"], sizeof(options.A0));
        strlcpy(options.I2C, configDocument["I2C"], sizeof(options.I2C));
    }

    configFile.close();
    return true;

}

/**
 * @brief Initializes the options struct with default values, which can be changed in the base script.
 * 
 */
void Config::initOptions() {

    strlcpy(options.device_id, "AAAA", sizeof(options.device_id));        
    options.version = 0;

    options.mqtt_interval = 4000;
    strlcpy(options.update_file, "update.bin", sizeof(options.update_file));

    options.carrier_temperature = true;
    options.carrier_humidity = true;
    options.carrier_pressure = true;
    options.carrier_gas = true;
    options.carrier_VOC = true;
    options.carrier_CO2 = true;
    options.carrier_acceleration = true;

    strlcpy(options.A6, "", sizeof(options.A6));
    strlcpy(options.A0, "", sizeof(options.A0));
    strlcpy(options.I2C, "", sizeof(options.I2C));

}


// /**
//  * @brief Prints the current options of the configuration to the serial monitor.
//  * 
//  */
// void Config::printOptions() {
//     Serial.println("Printing options struct:");

//     Serial.println("Device ID: " + String(options.device_id));
//     Serial.println("MQTT Interval: " + String(options.mqtt_interval));
//     Serial.println("Update File: " + String(options.update_file));
//     Serial.println("Carrier Temperature: " + String(options.carrier_temperature));
//     Serial.println("Carrier Humidity: " + String(options.carrier_humidity));
//     Serial.println("Carrier Pressure: " + String(options.carrier_pressure));
//     Serial.println("Carrier Gas: " + String(options.carrier_gas));
//     Serial.println("Carrier VOC: " + String(options.carrier_VOC));
//     Serial.println("Carrier CO2: " + String(options.carrier_CO2));
//     Serial.println("Carrier Acceleration: " + String(options.carrier_acceleration));
//     Serial.println("A6: " + String(options.A6));
//     Serial.println("A0: " + String(options.A0));
//     Serial.println("I2C: " + String(options.I2C));
// }

String Config::toString() {
    String optionsString = "";

    optionsString += "Device ID: " + String(options.device_id) + "\n";
    optionsString += "Version: " + String(options.version) + "\n";
    optionsString += "MQTT Interval: " + String(options.mqtt_interval) + "\n";
    optionsString += "Update File: " + String(options.update_file) + "\n";
    optionsString += "Carrier Temperature: " + String(options.carrier_temperature) + "\n";
    optionsString += "Carrier Humidity: " + String(options.carrier_humidity) + "\n";
    optionsString += "Carrier Pressure: " + String(options.carrier_pressure) + "\n";
    optionsString += "Carrier Gas: " + String(options.carrier_gas) + "\n";
    optionsString += "Carrier VOC: " + String(options.carrier_VOC) + "\n";
    optionsString += "Carrier CO2: " + String(options.carrier_CO2) + "\n";
    optionsString += "Carrier Acceleration: " + String(options.carrier_acceleration) + "\n";
    optionsString += "A6: " + String(options.A6) + "\n";
    optionsString += "A0: " + String(options.A0) + "\n";
    optionsString += "I2C: " + String(options.I2C) + "\n";

    return optionsString;
}