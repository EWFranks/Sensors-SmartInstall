
#ifndef SDCONFIG_H
#define SDCONFIG_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include <SD.h>

#include <EEPROM.h>


#define ID_ADDRESS 128 // 8 Bytes long
#define ID_LENGTH 8

#define INTERVAL_ADDRESS 136 // 4 Bytes long

/**
 * @brief Struct that holds the global configuration options that can be loaded and saved on an attached SD card.
 * 
 */
struct Options {
    char device_id[8]; // Should be 4 character id, but adding 8 to accomodate later naming schemes
    int mqtt_interval;
};

/**
 * @brief This class manages the configuration of the device, stored on the SD Card.
 * The Config object contains an Options struct, which holds all of the configuration options.
 * 
 */
class Config {
    public:
        Options options;
        Config();
        void begin();
        bool saveOptions();
        bool loadOptions();
        void initOptions();
        //void printOptions();
        String toString();
};

#endif