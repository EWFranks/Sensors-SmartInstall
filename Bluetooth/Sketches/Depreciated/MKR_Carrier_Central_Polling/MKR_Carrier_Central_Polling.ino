/*
Ok, I want this version of the carrier/wifi to connect two two periferals at the touch of a button, then poll through two periferals for information
*/


#include <ArduinoBLE.h>
#include <Arduino_MKRIoTCarrier.h>
#include "visuals.h"

#define BLE_MKR_UUID(val) ("19b10001-" val "-537e-4f6c-d104768a1214")

MKRIoTCarrier carrier;
const char* deviceServiceUuid = BLE_MKR_UUID("0000");
const char* deviceTempCharacteristicUuid = BLE_MKR_UUID("2001");
const char* deviceHumidityCharacteristicUuid = BLE_MKR_UUID("3001");
const char* devicePressureCharacteristicUuid = BLE_MKR_UUID("4001");

BLEFloatCharacteristic temperatureCharacteristic(BLE_MKR_UUID("2001"), BLERead);
BLEUnsignedIntCharacteristic humidityCharacteristic(BLE_MKR_UUID("3001"), BLERead);
BLEFloatCharacteristic pressureCharacteristic(BLE_MKR_UUID("4001"), BLERead);

//
unsigned long previousMillis = 0;          
const long interval = 5000; 

// Buttons stuff
int mode = 0;
uint32_t colorRed = carrier.leds.Color(200, 0, 0);
uint32_t colorGreen = carrier.leds.Color(0, 200, 0);
uint32_t colorBlue = carrier.leds.Color(0, 0, 200);

void setup() {
  // put your setup code here, to run once:
  CARRIER_CASE = false;
  Serial.begin(9600);
  delay(1500);


  if (!carrier.begin())
  {
    Serial.println("Carrier not connected, check connections");
    while (1);
  } 
    //initialize ArduinoBLE library
  if (!BLE.begin()) {
    Serial.println("starting Bluetooth® Low Energy failed!");
    while (1);
  }
  BLE.setLocalName("MKR WiFi 1010 (Central)");
  BLE.setDeviceName("MKR WiFi 1010 (Central)");
  BLE.advertise();

  Serial.println("MKR WiFi 1010 (Central Device)");
  Serial.println(" ");

}

void loop() {
  // put your main code here, to run repeatedly:
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval)
  {
    previousMillis = currentMillis;
    getReadings();
  }
}

void getReadings(){
  BLEDevice Peripheral = connectToPeripheral(UUID);
  display("a",Peripheral);
  Peripheral.disconnect();
  Peripheral = connectToPeripheral(UUID2);
  display("b",Peripheral);
  Peripheral.disconnect();
}

BLEDevice connectToPeripheral(char* DeviceUUID){
  BLEDevice peripheral;
  
  Serial.println("- Discovering peripheral device...");

  do
  {
    BLE.scanForUuid(DeviceUUID);
    peripheral = BLE.available();
  } while (!peripheral);
  
  if (peripheral) {
    Serial.println("* Peripheral device found!");
    Serial.print("* Device MAC address: ");
    Serial.println(peripheral.address());
    Serial.print("* Device name: ");
    Serial.println(peripheral.localName());
    Serial.print("* Advertised service UUID: ");
    Serial.println(peripheral.advertisedServiceUuid());
    Serial.println(" ");
    BLE.stopScan();
  }
  return peripheral;
}

void display(char Device, BLEDevice peripheral){
  carrier.display.fillScreen(0x0000);

  // displaying temperature
  carrier.display.setCursor(25, 60);
  carrier.display.setTextSize(3);
  carrier.display.print("Temperature");
  float temperature = peripheral.characteristic(deviceTempCharacteristicUuid);   //storing temperature value in a variable
  carrier.display.drawBitmap(80, 80, temperature_logo, 100, 100, 0xDAC9); //0xDA5B4A); //draw a thermometer on the MKR IoT carrier's display
  carrier.display.setCursor(60, 180);
  carrier.display.print(temperature); // display the temperature on the screen
  carrier.display.print(" C");
  delay(500);

  // displaying humidity
  carrier.display.fillScreen(0x0000);
  carrier.display.setCursor(54, 40);
  carrier.display.setTextSize(3);
  carrier.display.print("Humidity");
  carrier.display.drawBitmap(70, 70, humidity_logo, 100, 100, 0x0D14); //0x0CA1A6); //draw a humidity figure on the MKR IoT carrier's display
  float humidity = peripheral.characteristic(deviceHumidityCharacteristicUuid); //storing humidity value in a variable
  carrier.display.setCursor(60, 180);
  carrier.display.print(humidity); // display the humidity on the screen
  carrier.display.print(" %");
  delay(500);

  // displaying pressure
  carrier.display.fillScreen(0x0000);
  carrier.display.setCursor(54, 40);
  carrier.display.setTextSize(3);
  carrier.display.print("Pressure");
  carrier.display.drawBitmap(70, 60, pressure_logo, 100, 100, 0xF621); //0xF1C40F); //draw a pressure figure on the MKR IoT carrier's display
  float press = peripheral.characteristic(devicePressureCharacteristicUuid); //storing pressure value in a variable
  carrier.display.setCursor(40, 160);
  carrier.display.println(press); // display the pressure on the screen
  carrier.display.setCursor(160, 160);
  carrier.display.print("KPa");
  delay(500);
}

