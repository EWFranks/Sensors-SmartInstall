
#ifndef SDCONFIG_H
#define SDCONFIG_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include <SD.h>

/**
 * @brief Struct that holds the global configuration options that can be loaded and saved on an attached SD card.
 * 
 */
struct Options {
    char device_id[8]; // Should be 4 character id, but adding 8 to accomodate later naming schemes
    int version;

    int mqtt_interval;
    char update_file[32];
    
    bool carrier_temperature;
    bool carrier_humidity;
    bool carrier_pressure;
    bool carrier_gas;
    bool carrier_VOC;
    bool carrier_CO2;
    bool carrier_acceleration;

    char A6[16];
    char A0[16];
    char I2C[16];
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
        void begin(String filename);
        bool saveOptions();
        bool loadOptions();
        void initOptions();
        //void printOptions();
        String toString();
};

#endif