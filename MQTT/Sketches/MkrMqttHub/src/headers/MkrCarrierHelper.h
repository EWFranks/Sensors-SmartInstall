
#ifndef MKRCARRIERHELPER_H
#define MKRCARRIERHELPER_H

#include <Arduino.h>
#include <Arduino_MKRIoTCarrier.h>
#include <ArduinoJson.h>

// Relies on SdConfig.h for a single function
#include "SdConfig.h"

/**
 * @brief Simple class that wraps the MKRIoTCarrier library and provides helper functions for the MQTT sketch.
 * Has functions for reading the sensors on the carrier onto a JsonArray, and for controlling the screen, jingle, and LED.
 * 
 */
class MkrCarrierHelper {
    public:
        MkrCarrierHelper();
        MKRIoTCarrier getCarrier();
        void carrierSetup();
        void accelerationCarrierAvailable();
        void addReading(JsonArray readings, String readingType, String readingVal, String readingUnit);

        void addCarrierReadings(JsonArray readings, Config* config);
        void addCarrierTemperature(JsonArray readings);
        void addCarrierHumidity(JsonArray readings);
        void addCarrierPressure(JsonArray readings);
        void addCarrierGas(JsonArray readings);
        void addCarrierVoc(JsonArray readings);
        void addCarrierCo2(JsonArray readings);
        void addCarrierAcceleration(JsonArray readings);

        void screenHandler(String deviceID);
        void screenSet(String message);
        void screenAppend(String message);
        void resetScreen();

        void jinglePositive();
        void jingleNegative();
        void ledOn(int r, int g, int b);
        void ledOff();
    private:
        MKRIoTCarrier carrier;
        float accelX, accelY, accelZ;
        int cursorX, cursorY, yReset;
};

#endif
