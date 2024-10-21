// Variables needed for acceleration/color/motion

int red = -1;
int green = -1;
int blue = -1;

uint8_t motion = -100;

float accelX = -1;
float accelY = -1;
float accelZ = -1;


/**
 * @brief Adds the given sensor reading to the readings JsonArray, with the needed parameters.
 * 
 * @param readings The JsonArray which stores all sensor readings for the sending interval.
 * @param readingType The type of the reading, e.g. temperature, humidity, etc.
 * @param readingVal The value of the reading, e.g. 20.5, 30, etc.
 * @param readingUnit The unit of the reading, e.g. celsius, percent, etc.
 * @return The JsonObject containing the reading information.
 * Not actually used, since already added to the readings array.
 */
JsonObject addReading(JsonArray readings, String readingType, String readingVal, String readingUnit = "") {

  JsonObject readingObject = readings.createNestedObject();

  readingObject["reading_type"] = readingType;
  if(readingUnit != "")
    readingObject["reading_unit"] = readingUnit;
  
  readingObject["reading_val"] = readingVal;

  addReadingScreen(readingType, readingVal, readingUnit);

  
  return readingObject;

}

/**
 * @brief Add the reading to the carrier screen.
 * The readings information is displayed a vertical list on screen as it is collected.
 * 
 * @param readingType The type of the reading, e.g. temperature, humidity, etc.
 * @param readingVal The value of the reading, e.g. 20.5, 30, etc.
 * @param readingUnit The unit of the reading, e.g. celsius, percent, etc.
 */
void addReadingScreen(String readingType, String readingVal, String readingUnit) {
  carrier.display.setCursor(cursorX, cursorY);
  carrier.display.print(readingType + ": " + readingVal + " " + readingUnit);
  cursorY += 10;
}


// --------- AVAILABILITY COMMANDS (CALLED ONCE PER LOOP) -------------

// /**
//  * @brief Checks to see if any sensors are available to be read.
//  * Happens once per loop, outside of the MQTT sending interval.
//  * 
//  */
// void carrierAvailablity() {

//   accelerationCarrierAvailable();
//   // gestureCarrierAvailable();
//   // colorCarrierAvailable();

// }

void loopCalculations() {

  accelerationCarrierAvailable();



}

/**
 * @brief Checks to see if the acceleration sensor is available to be read.
 * Happens once per loop, outside of the MQTT sending interval.
 * 
 */
void accelerationCarrierAvailable() {
    if(carrier.IMUmodule.accelerationAvailable()) {
      carrier.IMUmodule.readAcceleration(accelX, accelY, accelZ);
    }
}

// void gestureCarrierAvailable() {
//     if(carrier.Light.gestureAvailable())
//       motion = carrier.Light.readGesture();
// }

// void colorCarrierAvailable() {
//     if(carrier.Light.colorAvailable()) {
//       carrier.Light.readColor(red, green, blue);
//     }
// }


// --------------- READING COMMANDS -----------------

/**
 * @brief Reads the temperature from the carrier, and adds it to the readings array.
 * 
 * @param readings The JsonArray which stores all sensor readings for the sending interval.
 */
void readCarrierTemperature(JsonArray readings) {

  float temp = carrier.Env.readTemperature();
  addReading(readings, "temperature", String(temp), "celsius"); // Read temperature

}

/**
 * @brief Reads the humidity from the carrier, and adds it to the readings array.
 * 
 * @param readings The JsonArray which stores all sensor readings for the sending interval.
 */
void readCarrierHumidity(JsonArray readings) {

  int humidity = carrier.Env.readHumidity();
  addReading(readings, "humidity", String( humidity ), "percent");

}

/**
 * @brief Reads the pressure from the carrier, and adds it to the readings array.
 * 
 * @param readings The JsonArray which stores all sensor readings for the sending interval.
 */
void readCarrierPressure(JsonArray readings) {

  int pressure = carrier.Pressure.readPressure();
  addReading(readings, "pressure", String( pressure ), "hectopascal");

}

/**
 * @brief Reads the gas resistor from the carrier, and adds it to the readings array.
 * 
 * @param readings The JsonArray which stores all sensor readings for the sending interval.
 */
void readCarrierGasResistor(JsonArray readings) {

  float gasResistor = carrier.AirQuality.readGasResistor();
  addReading(readings, "gas_resistor", String(gasResistor), "ppb");

}

/**
 * @brief Reads the VOC reading from the carrier, and adds it to the readings array.
 * 
 * @param readings The JsonArray which stores all sensor readings for the sending interval.
 */
void readCarrierVoc(JsonArray readings) {

  float volatileOrganicCompounds = carrier.AirQuality.readVOC();
  addReading(readings, "VOC", String(volatileOrganicCompounds), "ppm");
}

/**
 * @brief Reads the CO2 reading from the carrier, and adds it to the readings array.
 * 
 * @param readings The JsonArray which stores all sensor readings for the sending interval.
 */
void readCarrierCo2(JsonArray readings) {

  float co2 = carrier.AirQuality.readCO2();
  addReading(readings, "CO2", String(co2), "ppm");

}

/**
 * @brief Reads the acceleration from the carrier, and adds it to the readings array.
 * 
 * @param readings The JsonArray which stores all sensor readings for the sending interval.
 */
void readCarrierAcceleration(JsonArray readings) {

  String accelString = String(accelX) + "_" + String(accelY) + "_" + String(accelZ);
  addReading(readings, "acceleration", accelString, "v3");

    accelX = -1;
    accelY = -1;
    accelZ = -1;

}

/**
 * @brief Reads all the sensors from the carrier, and adds them to the readings array.
 * 
 * @param readings The JsonArray which stores all sensor readings for the sending interval.
 */
void readCarrier(JsonArray readings) {

    readCarrierTemperature(readings);
    readCarrierHumidity(readings);
    readCarrierPressure(readings);
    readCarrierGasResistor(readings);
    readCarrierVoc(readings);
    readCarrierCo2(readings);
    readCarrierAcceleration(readings);

}
    