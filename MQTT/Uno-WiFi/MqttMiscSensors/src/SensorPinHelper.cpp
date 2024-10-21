
/**
 * @file SensorPinHelper.cpp
 * @author Sterling Fisher (sjf2864@utulsa.edu)
 * @brief Creates a SensorPinHelper object that can be used to read sensors on pins A6, A0, I2C
 * Works for the MkrIoTCarrier. Should be more modular to allow for more pins for other sensors.
 * @version 0.1
 * @date 2023-10-27
 * 
 * @copyright Copyright (c) 2023
 * 
 */


#include <Arduino.h>
#include "headers\SensorPinHelper.h"






SensorPinHelper::SensorPinHelper() {}


/**
 * @brief Initiates the pointers needed for the SensorPinHelper object.
 * 
 * @param configPointer A pointer to the config object.
 * @param carrierHelperPointer An optional pointer to the MkrCarrierHelper object
 */
void SensorPinHelper::initPointers(Config* configPointer, MkrCarrierHelper* carrierHelperPointer=NULL) {

  prevFlameState = 0; // variable for switching flame reading
  
  prevPirState = false;
  intervalMotionDetected = 0; // variable for detecting if motion has been detected since reading last sent.

  _carrierHelper = carrierHelperPointer;

  String nameA6 = String(configPointer->options.A6);
  String nameA0 = String(configPointer->options.A0);
  String nameI2C = String(configPointer->options.I2C);
  
  chooseFunction(getA6, loopA6, nameA6); // Set function pointers for pin A6
  chooseFunction(getA0, loopA0, nameA0); // Set function pointers for pin A0
  chooseFunction(getI2C, loopI2C, nameI2C); // Set function pointers for I2C

// set pinmode if necessary for A6, A0, I2C

}


/**
 * @brief Sets the MkrCarrierHelper object used by SensorPinHelper.
 * 
 * @param carrierHelperPointer A pointer to the MkrCarrierHelper object. 
 */
void SensorPinHelper::setCarrierHelper(MkrCarrierHelper* carrierHelperPointer) {
  _carrierHelper = carrierHelperPointer;
}

/**
 * @brief Sets the given function pointers to the correct function based on the sensor name.
 * 
 * @param getFunction The function pointer that is ran every sending interval.
 * @param loopFunction The function pointer that is ran every loop. Usually for event-based sensors.
 * @param sensorName The name of the sensor attached.
 */
void SensorPinHelper::chooseFunction(SensorFunction& getFunction, LoopFunction& loopFunction, String sensorName) {
  
  // Control logic like a switch case on the sensor name 
  if(sensorName == "ultrasonic") {
    getFunction = &SensorPinHelper::addUltrasonic;
    loopFunction = NULL;
  }
  else if(sensorName == "flame") {
    getFunction = &SensorPinHelper::addFlame;
    loopFunction = NULL;
  }
  else if(sensorName == "water") {
    getFunction = &SensorPinHelper::addWater;
    loopFunction = NULL;
  }
  else if(sensorName == "gas") {
    getFunction = &SensorPinHelper::addGas;
    loopFunction = NULL;
  }
  else if(sensorName == "vibration") {
    getFunction = &SensorPinHelper::addVibration;
    loopFunction = NULL;
  }
  else if(sensorName == "pir") {
    getFunction = &SensorPinHelper::addPir;
    loopFunction = &SensorPinHelper::loopPir;
  }
  else { // If no sensor is attached or cannot be recognized, set function pointers to NULL
    getFunction = NULL;
    loopFunction = NULL;
  }

}

/**
 * @brief Called by the loop function.
 * Calls the loop function pointers for each pin, and checks if any function has had a detected event.
 * 
 * @return true if any function has had a detected event, 
 * @return false if there has been no event detected by any pin.
 */
bool SensorPinHelper::loop() {
  // Loop through all pins and call their pointer loop functions
  
  bool detectA6 = false;
  bool detectA0 = false;
  bool detectI2C = false;
  
  if(loopA6 != NULL) detectA6 = (this->*loopA6)(A6, -1);
  if(loopA0 != NULL) detectA0 = (this->*loopA0)(A0, -1);
  if(loopI2C != NULL) detectI2C = (this->*loopI2C)(SDA, SCL);

  bool infoDetected = detectA6 || detectA0 || detectI2C;
  
  return infoDetected;
}

/**
 * @brief Adds the sensor pin readings (if enabled in the config) to the given readings JsonArray.
 * 
 * @param readings The JsonArray which stores all sensor readings for the sending interval.
 */
void SensorPinHelper::addPinReadings(JsonArray readings) {
  if(getA6 != NULL) (this->*getA6)(readings, A6, -1); // -1 is unused output pin (carrier grove ports don't have output pins)
  if(getA0 != NULL) (this->*getA0)(readings, A0, -1);
  if(getI2C != NULL) (this->*getI2C)(readings, SDA, SCL); // SDA and SCL are the I2C pins
}



/**
 * @brief Adds the ultrasonic sensor reading to the given readings JsonArray.
 * 
 * @param readings The JsonArray which stores all sensor readings for the sending interval.
 * @param inputPin The input pin of the ultrasonic sensor.
 * @param outputPin The output pin of the ultrasonic sensor.
 * 
 */
void SensorPinHelper::addUltrasonic(JsonArray readings, int inputPin, int outputPin=-1) {
  Serial.println("ultrasonic start!");

  pinMode(inputPin, INPUT);
  pinMode(outputPin, OUTPUT); // Set output pin for ultrasonic sensor
  
  digitalWrite(outputPin, LOW); 
  delayMicroseconds(2);

  digitalWrite(outputPin, HIGH); // Pulse for 10μ s to trigger ultrasonic detection

  delayMicroseconds(10); 
  digitalWrite(outputPin, LOW);

  int distance = pulseIn(inputPin, HIGH); // Read receiver pulse time 
  distance= distance/58; // Transform pulse time to distance

  if(distance >= 300) return; // No object detected, don't send reading

  this->addReading(readings, "distance", String(distance), "cm");
}


/**
 * @brief Adds the flame sensor reading to the given readings JsonArray.
 * 
 * @param readings The JsonArray which stores all sensor readings for the sending interval.
 * @param inputPin The input pin of the flame sensor.
 * @param outputPin The output pin of the flame sensor.
 * 
 */
void SensorPinHelper::addFlame(JsonArray readings, int inputPin, int outputPin=-1) {
  int State = digitalRead(inputPin);
  
  if (State == HIGH && State != prevFlameState) {     
    // Send Flame on message 
    prevFlameState = HIGH;
    this->addReading(readings, "flame", "1", "flag");
  } 
  else if(State == LOW && State != prevFlameState) {
    // Send flame off message 
    prevFlameState = LOW;
    // continue function, returning false
  }
}


/**
 * @brief Adds the water sensor reading to the given readings JsonArray.
 * 
 * @param readings The JsonArray which stores all sensor readings for the sending interval.
 * @param inputPin The input pin of the water sensor.
 * @param outputPin The output pin of the water sensor.
 * 
 */
void SensorPinHelper::addWater(JsonArray readings, int inputPin, int outputPin=-1) {
  int level = analogRead(inputPin);
  this->addReading(readings, "water", String(map(level, 0, 1023, 0, 100)), "percent");
}


/**
 * @brief Adds the gas sensor reading to the given readings JsonArray.
 * 
 * @param readings The JsonArray which stores all sensor readings for the sending interval.
 * @param inputPin The input pin of the gas sensor.
 * @param outputPin The output pin of the gas sensor.
 * 
 */
void SensorPinHelper::addGas(JsonArray readings, int inputPin, int outputPin=-1) {
  int gasReading = analogRead(inputPin);
  this->addReading(readings, "gas", String(gasReading), "ppm");
}


/**
 * @brief Adds the vibration sensor reading to the given readings JsonArray.
 * 
 * @param readings The JsonArray which stores all sensor readings for the sending interval.
 * @param inputPin The input pin of the vibration sensor.
 * @param outputPin The output pin of the vibration sensor.
 * 
 */
void SensorPinHelper::addVibration(JsonArray readings, int inputPin, int outputPin=-1) {
  int vib = analogRead(inputPin);
  this->addReading(readings, "vibration", String(map(vib, 0, 1023, 0, 100)), "percent");
}


/**
 * @brief Adds the PIR sensor reading to the given readings JsonArray.
 * 
 * @param readings The JsonArray which stores all sensor readings for the sending interval.
 * @param inputPin The input pin of the PIR sensor.
 * @param outputPin The output pin of the PIR sensor.
 * 
 */
void SensorPinHelper::addPir(JsonArray readings, int inputPin, int outputPin=-1) {
  String readingString = String(intervalMotionDetected);

  if(prevPirState != intervalMotionDetected) {
    this->addReading(readings, "motion", readingString, "flag");
    prevPirState = intervalMotionDetected;
  }

  intervalMotionDetected = false; // Reset intervalMotionDetected to false
}


/**
 * @brief Adds a reading to the given readings JsonArray.
 * 
 * @param readings The JsonArray which stores all sensor readings for the sending interval.
 * @param readingType The type of reading (i.e. "temperature" or "humidity").
 * @param readingVal The value of the reading as a String.
 * @param readingUnit The unit of the reading.
 */
void SensorPinHelper::addReading(JsonArray readings, String readingType, String readingVal, String readingUnit) {
  
  // If carrier helper is set, just use its addReading function
  if(_carrierHelper != NULL)
    _carrierHelper->addReading(readings, readingType, readingVal, readingUnit);

  // Otherwise, add the reading manually
  else {
    JsonObject readingObject = readings.createNestedObject(); // Create a new reading object in the readings array

    readingObject["reading_type"] = readingType;
    
    // Special case if reading doesn't have a unit
    if(readingUnit != "")
      readingObject["reading_unit"] = readingUnit;
    
    readingObject["reading_val"] = readingVal;
  
  }
}


/**
 * @brief Called by the loop function.
 * Checks if the PIR sensor has detected motion.
 * 
 * @param inputPin The input pin of the PIR sensor.
 * @param outputPin The output pin of the PIR sensor.
 * @return true if motion has been detected since the previous sending interval,
 * @return false if no motion has been detected in the current interval.
 */
bool SensorPinHelper::loopPir(int inputPin, int outputPin=-1) {
  int pir_motion = analogRead(inputPin);
  int threshold = 200;

  if(pir_motion > threshold) {
    intervalMotionDetected = true; // Starts off each interval false, if motion is ever detected before sending interval, set to true
    if(_carrierHelper != NULL) _carrierHelper->ledOn(0,200,0); // Turn on green LED
  }

  if(!intervalMotionDetected && _carrierHelper != NULL)
    _carrierHelper->ledOff(); // Turn off green LED
  
  return intervalMotionDetected;

}