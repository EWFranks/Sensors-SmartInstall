/**
 * @file CommandHandler.cpp
 * @author Sterling Fisher (sjf2864@utulsa.edu)
 * @brief Wraps the ArduinoMqttClient library to handle MQTT communication. Handles publishing readings and receiving and acting upon commands.
 * @version 0.1
 * @date 2023-10-31
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "headers\CommandHandler.h"




CommandHandler::CommandHandler() {}


/**
 * @brief Initializes the pointers to the config and carrierHelper objects.
 * Needed to access and modify the config and carrier from this class.
 * 
 * @param config The pointer to the config object from the main file.
 * @param carrierHelper The pointer to the carrierHelper object from the main file.
 */
void CommandHandler::initPointers(Config* configPointer, MkrCarrierHelper* carrierHelperPointer) {

    // Referencing existing config and carrierHelper objects from main file
    config = configPointer;
    carrierHelper = carrierHelperPointer;
}




/**
 * @brief Handles the command received through MQTT.
 * 
 * @param commandTopic The topic of the command received.
 * @param message The message of the command received.
 */
void CommandHandler::handleCommand(String commandTopic, String message) {

  String command = commandTopic.substring(14); // Extract the command from the topic
  Serial.println("command received: " + command);

  decideCommand(command, message);

  //quickPublish("data/config/" + String(config->options.device_id), config->toString());
  config->saveOptions();

}


/**
 * @brief Given a command name, it determines the functionality to be executed.
 * 
 * @param command The command to be handled.
 */
void CommandHandler::decideCommand(String command, String commandData) {
  if (command == "restart") restartBoard();
  else if (command == "jingle") carrierHelper->jinglePositive();
  //else if (command == "check_update") handleSketchDownload();
  //else if (command == "force_update") forceUpdate();

  else if (command == "set/id") setID(commandData);
  else if (command == "set/directory") setUpdateFile(commandData);
  
  else if (command == "set/interval")
    config->options.mqtt_interval = commandData.toInt();

  // Enable/disable the carrier readings:
  // If message is false, disable. Else, have it enabled.
  // Default state is enabled. If not given false, it is enabled.
  else if (command == "set/carrier_temperature_enabled")
    config->options.carrier_temperature = (commandData != "false");
  else if (command == "set/carrier_humidity_enabled")
    config->options.carrier_humidity = (commandData != "false");
  else if (command == "set/carrier_pressure_enabled")
    config->options.carrier_pressure = (commandData != "false");
  else if (command == "set/carrier_gas_enabled")
    config->options.carrier_gas = (commandData != "false");
  else if (command == "set/carrier_VOC_enabled")
    config->options.carrier_VOC = (commandData != "false");
  else if (command == "set/carrier_CO2_enabled")
    config->options.carrier_CO2 = (commandData != "false");
  else if (command == "set/carrier_acceleration_enabled")
    config->options.carrier_acceleration = (commandData != "false");
  
  // Set the carrier pins:
  else if (command == "set/A6")
    commandData.toCharArray(config->options.A6, sizeof(config->options.A6));
  else if (command == "set/A0")
    commandData.toCharArray(config->options.A0, sizeof(config->options.A0));
  else if (command == "set/I2C")
    commandData.toCharArray(config->options.I2C, sizeof(config->options.I2C));

  // Predefined profiles of settings:
  else if(command == "profile/general") setGeneralProfile();
  else if(command == "profile/pir") setPirProfile();
}




/**
 * @brief Sets the settings to the general profile.
 * 
 */
void CommandHandler::setGeneralProfile() {
    config->options.mqtt_interval = 4000;

    config->options.carrier_temperature = true;
    config->options.carrier_humidity = true;
    config->options.carrier_pressure = true;
    config->options.carrier_gas = true;
    config->options.carrier_VOC = true;
    config->options.carrier_CO2 = true;
    config->options.carrier_acceleration = true;

    strlcpy(config->options.A6, "", sizeof(config->options.A6));
    strlcpy(config->options.A0, "", sizeof(config->options.A0));
    strlcpy(config->options.I2C, "", sizeof(config->options.I2C));

    restartBoard();
}


/**
 * @brief Sets the settings to the PIR profile.
 * 
 */
void CommandHandler::setPirProfile() {
  config->options.mqtt_interval = 1000;

  config->options.carrier_temperature = false;
  config->options.carrier_humidity = false;
  config->options.carrier_pressure = false;
  config->options.carrier_gas = false;
  config->options.carrier_VOC = false;
  config->options.carrier_CO2 = false;
  config->options.carrier_acceleration = false;

  strlcpy(config->options.A6, "", sizeof(config->options.A6));
  strlcpy(config->options.A0, "pir", sizeof(config->options.A0));
  strlcpy(config->options.I2C, "", sizeof(config->options.I2C));

  restartBoard();
}




/**
 * @brief Restarts the board.
 * 
 */
void CommandHandler::restartBoard() {
  config->saveOptions();
  //quickPublish("events/" + String(config->options.device_id), "restart activated by event.");
  carrierHelper->jingleNegative();

  NVIC_SystemReset();
}


/**
 * @brief Sets the device ID to the given ID.
 * 
 * @param newID The new ID to be set.
 */
void CommandHandler::setID(String newID) {
  newID.toCharArray(config->options.device_id, sizeof(config->options.device_id));
  restartBoard();
}


/**
 * @brief Sets the update file to the given update file.
 * 
 * @param newUpdateFile The new update file to be set.
 */
void CommandHandler::setUpdateFile(String newUpdateFile) {
  newUpdateFile.toCharArray(config->options.update_file, sizeof(config->options.update_file));
  restartBoard();
}

/**
 * @brief Sets the MQTT interval to the given interval.
 * 
 * @param newInterval The new interval to be set.
 */
void CommandHandler::setInterval(int newInterval) { 
  config->options.mqtt_interval = newInterval;
  }