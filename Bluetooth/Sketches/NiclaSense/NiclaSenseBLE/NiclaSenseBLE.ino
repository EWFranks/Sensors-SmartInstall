
// Author: Sterling Fisher
// Based on https://docs.arduino.cc/tutorials/nicla-sense-me/web-ble-dashboard script

#include "Nicla_System.h"
#include "Arduino_BHY2.h"
#include <ArduinoBLE.h>

#include "config.h"


// Defines
  #define BLE_SENSE_UUID(val) ("19b10000-" val "-537e-4f6c-d104768a1214")

// Enter in Config
  char deviceName[] = BLE_NAME;

// Interval Variables
  const long interval = 1; // Interval for sending messages in milliseconds
  unsigned long currentMillis = 0;
  unsigned long previousMillis = 0;

// BLE Variables
  BLEService service(BLE_SENSE_UUID("0000")); // create service

  //BLECharacteristic exampleCharacteristic("19B10012-E8F2-537E-4F6C-D104768A1214",   // Create characteristic
  //                                        BLERead | BLENotify, "00000000000000000000000000000000");

  BLECharacteristic versionCharacteristic(BLE_SENSE_UUID("1001"), BLERead | BLENotify, 32 * sizeof(char));
  BLECharacteristic temperatureCharacteristic(BLE_SENSE_UUID("2001"), BLERead | BLENotify, 32 * sizeof(char));
  BLECharacteristic humidityCharacteristic(BLE_SENSE_UUID("3001"), BLERead | BLENotify, 32 * sizeof(char));
  BLECharacteristic pressureCharacteristic(BLE_SENSE_UUID("4001"), BLERead | BLENotify, 32 * sizeof(char));

  BLECharacteristic accelerometerCharacteristic(BLE_SENSE_UUID("5001"), BLERead | BLENotify, 32 * sizeof(char));  // Array of 3x 2 Bytes, XY
  BLECharacteristic gyroscopeCharacteristic(BLE_SENSE_UUID("6001"), BLERead | BLENotify, 32 * sizeof(char));    // Array of 3x 2 Bytes, XYZ
  BLECharacteristic quaternionCharacteristic(BLE_SENSE_UUID("7001"), BLERead | BLENotify, 32 * sizeof(char));     // Array of 4x 2 Bytes, XYZW

  BLECharacteristic rgbLedCharacteristic(BLE_SENSE_UUID("8001"), BLERead | BLEWrite, 3 * sizeof(byte)); // Array of 3 bytes, RGB

  BLECharacteristic bsecCharacteristic(BLE_SENSE_UUID("9001"), BLERead | BLENotify, 32 * sizeof(char));
  BLECharacteristic co2Characteristic(BLE_SENSE_UUID("9002"), BLERead | BLENotify, 32 * sizeof(char));
  BLECharacteristic gasCharacteristic(BLE_SENSE_UUID("9003"), BLERead | BLENotify, 32 * sizeof(char));


// Nicla Sense ME Variables
  String bleName;

  Sensor temperature(SENSOR_ID_TEMP);
  Sensor humidity(SENSOR_ID_HUM);
  Sensor pressure(SENSOR_ID_BARO);
  Sensor gas(SENSOR_ID_GAS);
  SensorXYZ gyroscope(SENSOR_ID_GYRO);
  SensorXYZ accelerometer(SENSOR_ID_ACC);
  SensorQuaternion quaternion(SENSOR_ID_RV);
  SensorBSEC bsec(SENSOR_ID_BSEC);



void setup() {
  // Runs at start of sketch, initializes functionality.
  
  Serial.begin(9600);

  initializeNicla();
  initializeBLE();

}


void initializeBLE() {
  // Initialize the BLE functionality, and set services and corresponding characteristics

  initializeNicla();
  
  while(!BLE.begin()) { // begin initialization
    Serial.println("starting Bluetooth® Low Energy module failed!");
  }

  String address = BLE.address();
  address.toUpperCase();

  bleName = deviceName;
  bleName += ":";
  bleName += address[address.length() - 5];
  bleName += address[address.length() - 4];
  bleName += address[address.length() - 2];
  bleName += address[address.length() - 1];

  BLE.setLocalName(bleName.c_str()); // set the local name peripheral advertises

  // Manage services and service characteristics  
  BLE.setAdvertisedService(service);

  // Add all the previously defined Characteristics
    service.addCharacteristic(temperatureCharacteristic);
    service.addCharacteristic(humidityCharacteristic);
    service.addCharacteristic(pressureCharacteristic);
    service.addCharacteristic(versionCharacteristic);
    service.addCharacteristic(accelerometerCharacteristic);
    service.addCharacteristic(gyroscopeCharacteristic);
    service.addCharacteristic(quaternionCharacteristic);
    service.addCharacteristic(bsecCharacteristic);
    service.addCharacteristic(co2Characteristic);
    service.addCharacteristic(gasCharacteristic);
    service.addCharacteristic(rgbLedCharacteristic);
  
  BLE.addService(service); // add the service
  
  BLE.advertise(); // start advertising
  Serial.println("Bluetooth® device active, waiting for connections...");

}

void initializeNicla() {
  nicla::begin();
  nicla::leds.begin();
  nicla::leds.setColor(green);

  //Sensors initialization
  BHY2.begin(NICLA_STANDALONE);
  temperature.begin();
  humidity.begin();
  pressure.begin();
  gyroscope.begin();
  accelerometer.begin();
  quaternion.begin();
  bsec.begin();
  gas.begin();
}


void loop() {
  
  BLE.poll(); // poll for BLE events
  BHY2.update();

    if(intervalCheck()) {  // If it is time to read data again
    previousMillis = currentMillis; // Saves last time message was sent

    bleHandler();

  }

}


void bleHandler() {

  publishTemperature();
  publishHumidity();
  publishPressure();

  publishAccelerometer();
  publishGyroscope();
  publishQuaternion();

  publishBsec();
  publishCo2();
  publishGas();


}


bool intervalCheck() {
  currentMillis = millis();
  return currentMillis - previousMillis >= interval;
}
