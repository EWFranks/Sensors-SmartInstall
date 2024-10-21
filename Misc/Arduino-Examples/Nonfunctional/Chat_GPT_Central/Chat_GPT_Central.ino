//Attempt a la chat GPT (Central)

#include <ArduinoBLE.h>

void setup() {
  Serial.begin(9600);
  while (!Serial);
  
  if (!BLE.begin()) {
    Serial.println("Failed to initialize BLE!");
    while (1);
  }
  
  BLE.scanForUuid("180A"); // Change UUID to match peripheral board
  
  Serial.println("Scanning for BLE peripherals...");
}

void loop() {
  BLEDevice peripheral = BLE.available();

  if (peripheral) {
    Serial.println("Peripheral found! Connecting...");

    if (peripheral.connect()) {
      Serial.println("Connected to peripheral!");
      // Perform any required actions with the peripheral
      // Disconnect from peripheral
      while(1);
      Serial.println("Disconnected from peripheral");
    } else {
      Serial.println("Failed to connect to peripheral!");
    }
  }
}
