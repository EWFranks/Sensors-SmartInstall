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
 * @brief Initializes the pointers to the config object.
 * Needed to access and modify the config from this class.
 * 
 * @param config The pointer to the config object from the main file.
 */
void CommandHandler::initPointers(Config* configPointer) {
    config = configPointer; // Referencing existing config from main file
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
  else if (command == "set/id") setID(commandData);
  else if (command == "set/interval") setInterval(commandData.toInt());

}


/**
 * @brief Restarts the board.
 * 
 */
void CommandHandler::restartBoard() {
  config->saveOptions();
  //quickPublish("events/" + String(config->options.device_id), "restart activated by event.");

  this->resetFunc();
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
 * @brief Sets the MQTT interval to the given interval.
 * 
 * @param newInterval The new interval to be set.
 */
void CommandHandler::setInterval(int newInterval) { 
  config->options.mqtt_interval = newInterval;
  }