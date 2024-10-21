
/**
 * @file MkrCarrierHelper.cpp
 * @author Sterling Fisher (sjf2864@utulsa.edu)
 * 
 * @brief This file manages the MKRIoTCarrier, and all of its sensors.
 * It holds the actual carrier object, and all of the functions to read the sensors.
 * 
 * @version 0.1
 * @date 2023-10-19
 * 
 */


#include <Arduino.h>
#include <Arduino_MKRIoTCarrier.h>
#include <ArduinoJson.h>

#include "headers\MkrCarrierHelper.h"






    // TASK A) Carrier object management:

//MKRIoTCarrier carrier;

MkrCarrierHelper::MkrCarrierHelper() {
  cursorX = 30; cursorY = 80; yReset = 80;
  accelX = 0; accelY = 0; accelZ = 0;
}


MKRIoTCarrier MkrCarrierHelper::getCarrier() {
  return carrier;
}


/**
 * @brief Checks to see if the carrier is connected, and initializes the sensors.
 * 
 */
void MkrCarrierHelper::carrierSetup() {

  carrier.noCase();

  if(!carrier.begin()) {
    Serial.println("Carrier not connected, check connections");
    while(1);
  }

  carrier.Light.begin();
  carrier.Light.setGestureSensitivity(100);

  Serial.println("Carrier is setup!");

}


/**
 * @brief Checks to see if the acceleration sensor is available to be read.
 * Should happen once per loop, not within the actual reading interval.
 * 
 */
void MkrCarrierHelper::accelerationCarrierAvailable() {
    if(carrier.IMUmodule.accelerationAvailable()) {
      carrier.IMUmodule.readAcceleration(accelX, accelY, accelZ);
    }
}



/**
 * @brief Adds the given sensor reading to the readings JsonArray, with the needed parameters.
 * 
 * @param readings The JsonArray which stores all sensor readings for the sending interval.
 * @param readingType The type of the reading, e.g. temperature, humidity, etc.
 * @param readingVal The value of the reading, e.g. 20.5, 30, etc.
 * @param readingUnit The unit of the reading, e.g. celsius, percent, etc.
 */
void MkrCarrierHelper::addReading(JsonArray readings, String readingType, String readingVal, String readingUnit) {
  JsonObject readingObject = readings.createNestedObject(); // Create a new reading object in the readings array

  readingObject["reading_type"] = readingType;
  if(readingUnit != "")
    readingObject["reading_unit"] = readingUnit;
  
  readingObject["reading_val"] = readingVal;

  String screenReading = readingType + ": " + readingVal + " " + readingUnit;
  screenAppend(screenReading);

}

/**
 * @brief Adds the carrier sensor readings (if enabled in the config) to the given readings JsonArray.
 * 
 * @param readings The JsonArray which stores all sensor readings for the sending interval.
 */
void MkrCarrierHelper::addCarrierReadings(JsonArray readings, Config* configPointer) {

  if(configPointer->options.carrier_temperature) addCarrierTemperature(readings);
  if(configPointer->options.carrier_humidity) addCarrierHumidity(readings);
  if(configPointer->options.carrier_pressure) addCarrierPressure(readings);
  if(configPointer->options.carrier_gas) addCarrierGas(readings);
  if(configPointer->options.carrier_VOC) addCarrierVoc(readings);
  if(configPointer->options.carrier_CO2) addCarrierCo2(readings);
  if(configPointer->options.carrier_acceleration) addCarrierAcceleration(readings);
}


// Refactoring previous methods to use Json arrays

/**
 * @brief Reads the temperature sensor on the carrier and adds it to the readings array.
 * 
 * @param readings The JsonArray which stores all sensor readings for the sending interval.
 */
void MkrCarrierHelper::addCarrierTemperature(JsonArray readings) {
  addReading(readings, "temperature", String(carrier.Env.readTemperature()), "celsius");
}


/**
 * @brief Reads the humidity sensor on the carrier and adds it to the readings array.
 * 
 * @param readings The JsonArray which stores all sensor readings for the sending interval.
 */
void MkrCarrierHelper::addCarrierHumidity(JsonArray readings) {
  addReading(readings, "humidity", String(carrier.Env.readHumidity()), "percent");
}


/**
 * @brief Reads the pressure sensor on the carrier and adds it to the readings array.
 * 
 * @param readings The JsonArray which stores all sensor readings for the sending interval.
 */
void MkrCarrierHelper::addCarrierPressure(JsonArray readings) {
  addReading(readings, "pressure", String(carrier.Pressure.readPressure()), "hectopascals");
}

/**
 * @brief Reads the gas resistor sensor on the carrier and adds it to the readings array.
 * 
 * @param readings The JsonArray which stores all sensor readings for the sending interval.
 * 
 */
void MkrCarrierHelper::addCarrierGas(JsonArray readings) {
  addReading(readings, "gas_resistor", String(carrier.AirQuality.readGasResistor()), "ppb");
}


/**
 * @brief Reads the VOC sensor on the carrier and adds it to the readings array.
 * 
 * @param readings The JsonArray which stores all sensor readings for the sending interval.
 * 
 */
void MkrCarrierHelper::addCarrierVoc(JsonArray readings) {
  addReading(readings, "VOC", String(carrier.AirQuality.readVOC()), "ppm");
}

/**
 * @brief Reads the CO2 sensor on the carrier and adds it to the readings array.
 * 
 * @param readings The JsonArray which stores all sensor readings for the sending interval.
 * 
 */
void MkrCarrierHelper::addCarrierCo2(JsonArray readings) {
  addReading(readings, "CO2", String(carrier.AirQuality.readCO2()), "ppm");
}

/**
 * @brief Reads the acceleration sensor on the carrier and adds it to the readings array.
 * 
 * @param readings The JsonArray which stores all sensor readings for the sending interval.
 * 
*/
void MkrCarrierHelper::addCarrierAcceleration(JsonArray readings) {
  String accelString = String(accelX) + "_" + String(accelY) + "_" + String(accelZ);
  
  addReading(readings, "acceleration", accelString, "v3");

  accelX = 0; accelY = 0; accelZ = 0;

}



    // TASK C) Manage carrier screen:

// Screen variables

  //int yReset = 80; // Pixels from top of screen to start writing




/**
 * @brief Handles the display every loop.
 * Clears the screen, set text settings, and write device ID.
 * @param deviceID The ID of the device as a String.
 * 
 */
void MkrCarrierHelper::screenHandler(String deviceID) {

  resetScreen(); // Clear screen

  // Write ID
  carrier.display.setTextColor(0xF800); // Set text color to red
  carrier.display.setTextSize(2);
  carrier.display.print("Device ID: " + deviceID);

  carrier.display.setTextColor(ST77XX_WHITE); // Change text color to white for readings
  carrier.display.setTextSize(1); // Shrink reading text

  cursorY += 30;  // Change text vertical position for next line

}




/**
 * @brief Prints the given message to the carrier screen.
 * @param message The String to be printed.
 */
void MkrCarrierHelper::screenSet(String message) {
  carrier.display.fillScreen(ST77XX_BLACK); // Clear screen
  cursorY = yReset; // Reset text vertical position to close to top of the screen
  carrier.display.setCursor(cursorX, cursorY); // Set text horizontal and vertical position

  // Write ID
  carrier.display.setTextColor(0xF800); // Set text color to red
  carrier.display.setTextSize(1);
  carrier.display.print(message);

//   carrier.display.setTextColor(ST77XX_WHITE); // Change text color to white for readings
//   carrier.display.setTextSize(1); // Shrink reading text
}


/**
 * @brief Adds the given message to the carrier screen's existing message.
 * @param message The String to be printed.
 */
void MkrCarrierHelper::screenAppend(String message) {
    // Ensure reading text settings:
   carrier.display.setTextColor(ST77XX_WHITE); // Change text color to white for readings
   carrier.display.setTextSize(1); // Shrink reading text


  carrier.display.setCursor(cursorX, cursorY);
  carrier.display.print(message);
  cursorY += 10;
}


/**
 * @brief Resets the carrier screen.
 */
void MkrCarrierHelper::resetScreen() {
    carrier.display.fillScreen(ST77XX_BLACK); // Clear screen
    cursorY = yReset; // Reset text vertical position to close to top of the screen
    carrier.display.setCursor(cursorX, cursorY); // Set text horizontal and vertical position
}


/**
 * @brief Plays an optimistic jingle on the carrier buzzer.  
 * 
 */
void MkrCarrierHelper::jinglePositive() {
  carrier.Buzzer.beep(600, 200);
  carrier.Buzzer.beep(400, 200);
  carrier.Buzzer.beep(800, 200);
}


/**
 * @brief Plays a pessimistic jingle on the carrier buzzer.
 * 
 */
void MkrCarrierHelper::jingleNegative() {
  carrier.Buzzer.beep(800, 200);
  carrier.Buzzer.beep(400, 200);
  carrier.Buzzer.beep(200, 200);
}


/**
 * @brief Turns on the LEDs of the carrier with the given rgb values
 * 
 */
void MkrCarrierHelper::ledOn(int r, int g, int b) {
  carrier.leds.fill(carrier.leds.Color(r,g,b), 0, 5);
  carrier.leds.show();
}

/**
 * @brief Turns off the LEDs of the carrier
 * 
 */
void MkrCarrierHelper::ledOff() {
  // carrier.leds.fill(carrier.leds.Color(0,0,0), 0, 5);
  // carrier.leds.show();
  carrier.leds.clear();
  carrier.leds.show();
}