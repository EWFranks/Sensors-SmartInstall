//attempt a la chat GPT (Periferal)

#include <Arduino_MKRIoTCarrier.h>
#include <ArduinoBLE.h>

MKRIoTCarrier carrier;

BLEService customService("19B10000-E8F2-537E-4F6C-D104768A1214"); // Change UUID to match central board
BLEStringCharacteristic customCharacteristic("19B10001-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite, 20); // Change UUID to match central board and adjust valueSize as per your requirements

void setup() {
  Serial.begin(9600);
  while (!Serial);
  
  if (!BLE.begin()) {
    Serial.println("Failed to initialize BLE!");
    while (1);
  }
  
  BLE.setLocalName("PeripheralDevice");
  BLE.setAdvertisedService(customService);
  customService.addCharacteristic(customCharacteristic);
  BLE.addService(customService);
  customCharacteristic.writeValue("Hello, Central!"); // Initial value for the characteristic
  
  BLE.advertise();
  Serial.println("BLE peripheral advertising...");
}

void loop() {
  // Check for central device connection
  BLEDevice central = BLE.central();

  if (central) {
    Serial.print("Connected to central: ");
    Serial.println(central.address());

    while (central.connected()) {
      // Read temperature from the sensor
      float temperature = carrier.Env.readTemperature();   //storing temperature value in a variable
      Serial.print("Temperature: ");
      Serial.print(temperature);
      Serial.println(" °C");
      
      // Convert temperature to string
      String temperatureString = String(temperature, 2);
      
      // Update the characteristic value
      customCharacteristic.writeValue(temperatureString);
      Serial.print("Sent value: ");
      Serial.println(temperatureString);
      
      delay(2000); // Wait for 2 seconds before sending the next temperature reading
    }

    Serial.print("Disconnected from central: ");
    Serial.println(central.address());
  }
}
