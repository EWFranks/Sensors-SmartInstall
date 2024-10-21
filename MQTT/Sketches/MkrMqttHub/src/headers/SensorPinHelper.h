#ifndef SENSORPINHELPER_H
#define SENSORPINHELPER_H

#include <Arduino.h>
#include <ArduinoJson.h>

#include "MkrCarrierHelper.h"
#include "SdConfig.h"

/**
 * @brief This class is used to help with reading from the sensor pins A6, A0, and the I2C port.
 * 
 */
class SensorPinHelper {
    public:
        typedef void (SensorPinHelper::*SensorFunction) (JsonArray readings, int inputPin, int outputPin); // Runs when sending readings for sensors
        typedef bool(SensorPinHelper::*LoopFunction) (int inputPin, int outputPin); // Runs during base loop

        SensorPinHelper();

        void initPointers(Config* config, MkrCarrierHelper* carrierHelper);
        void chooseFunction(SensorFunction& getFunction, LoopFunction& loopFunction, String sensorName);
        bool loop();
        void setCarrierHelper(MkrCarrierHelper* carrierHelperPtr);

        void addPinReadings(JsonArray readings);


    private:
        int prevFlameState;

        bool prevPirState;
        bool intervalMotionDetected;


        SensorFunction getA6;
        SensorFunction getA0;
        SensorFunction getI2C;

        LoopFunction loopA6;
        LoopFunction loopA0;
        LoopFunction loopI2C;

        MkrCarrierHelper* _carrierHelper;

        void addUltrasonic(JsonArray readings, int inputPin, int outputPin);
        void addFlame(JsonArray readings, int inputPin, int outputPin);
        void addWater(JsonArray readings, int inputPin, int outputPin);
        void addGas(JsonArray readings, int inputPin, int outputPin);
        void addVibration(JsonArray readings, int inputPin, int outputPin);
        void addPir(JsonArray readings, int inputPin, int outputPin);
        void addReading(JsonArray readings, String readingType, String readingVal, String readingUnit);
        bool loopPir(int inputPin, int outputPin);
};

#endif