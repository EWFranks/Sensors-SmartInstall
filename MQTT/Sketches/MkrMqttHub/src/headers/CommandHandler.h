#ifndef COMMAND_HANDLER_H
#define COMMAND_HANDLER_H



#include "SdConfig.h"
#include "MkrCarrierHelper.h"


#include "..\libraries\ArduinoMqttclient\ArduinoMqttClient.h"
#include <Arduino.h>

#include <ArduinoJson.h>
#include <WiFiNINA.h>


/**
 * @brief This class handles the logic for handling commands sent through MQTT.
 * This class modifies the config object, and the carrierHelper object. initPointers() must be called before using the commands in this class.
 * 
 */
class CommandHandler {
    public:
        Config* config;
        MkrCarrierHelper* carrierHelper;

        CommandHandler();

        void initPointers(Config* config, MkrCarrierHelper* carrierHelper);
        void handleCommand(String commandTopic, String message);

    private:
        void decideCommand(String command, String commandData);
        
        void setGeneralProfile();
        void setPirProfile();
        void restartBoard();
        void setID(String newID);
        void setUpdateFile(String newUpdateFile);
        void setInterval(int newInterval);

};



#endif