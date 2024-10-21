// Variables needed for acceleration/color/motion

int red = -1;
int green = -1;
int blue = -1;

uint8_t motion = -100;

float accelX = -1;
float accelY = -1;
float accelZ = -1;



JsonObject addReading(JsonArray readings, String readingType, String readingVal, String readingUnit) {

  JsonObject readingObject = readings.createNestedObject();

  readingObject["reading_type"] = readingType;
  if(readingUnit != "")
    readingObject["reading_unit"] = readingUnit;
  
  readingObject["reading_val"] = readingVal;

  addReadingScreen(readingType, readingVal, readingUnit);

  
  return readingObject;

}


void addReadingScreen(String readingType, String readingVal, String readingUnit) {
  carrier.display.setCursor(cursorX, cursorY);
  carrier.display.print(readingType + ": " + readingVal + " " + readingUnit);
  cursorY += 10;
}


// --------- AVAILABILITY COMMANDS (CALLED ONCE PER LOOP) -------------

void carrierAvailablity() {

  accelerationCarrierAvailable();
  // gestureCarrierAvailable();
  // colorCarrierAvailable();

}

void accelerationCarrierAvailable() {
    if(carrier.IMUmodule.accelerationAvailable()) {
      carrier.IMUmodule.readAcceleration(accelX, accelY, accelZ);
    }
}


// --------------- READING COMMANDS -----------------

void readCarrierTemperature(JsonArray readings) {

  float temp = carrier.Env.readTemperature();
  addReading(readings, "temperature", String(temp), "celsius"); // Read temperature

}


void readCarrierHumidity(JsonArray readings) {

  int humidity = carrier.Env.readHumidity();
  addReading(readings, "humidity", String( humidity ), "percent");

}


void readCarrierPressure(JsonArray readings) {

  int pressure = carrier.Pressure.readPressure();
  addReading(readings, "pressure", String( pressure ), "hectopascal");

}


void readCarrierGasResistor(JsonArray readings) {

  float gasResistor = carrier.AirQuality.readGasResistor();
  addReading(readings, "gas_resistor", String(gasResistor), "ppb");

}


void readCarrierVoc(JsonArray readings) {

  float volatileOrganicCompounds = carrier.AirQuality.readVOC();
  addReading(readings, "VOC", String(volatileOrganicCompounds), "ppm");
}


void readCarrierCo2(JsonArray readings) {

  float co2 = carrier.AirQuality.readCO2();
  addReading(readings, "CO2", String(co2), "ppm");

}


void readCarrierAcceleration(JsonArray readings) {

  String accelString = String(accelX) + "_" + String(accelY) + "_" + String(accelZ);
  addReading(readings, "acceleration", accelString, "v3");

    accelX = -1;
    accelY = -1;
    accelZ = -1;

}

void readCarrier(JsonArray readings) {

    readCarrierTemperature(readings);
    readCarrierHumidity(readings);
    readCarrierPressure(readings);
    readCarrierGasResistor(readings);
    readCarrierVoc(readings);
    readCarrierCo2(readings);
    readCarrierAcceleration(readings);

}
    