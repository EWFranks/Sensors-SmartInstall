#include <Arduino_MKRIoTCarrier.h>


// Define pin numbers for your sensors
const int WATER_SENSOR_PIN = 6; // Replace with your water level sensor pin
const int AIR_SENSOR_PIN = 7;   // Replace with your air quality sensor pin

void setup() {
  // Initialize serial communication for debugging
  Serial.begin(9600);

  // Set sensor pins as inputs
  pinMode(WATER_SENSOR_PIN, INPUT);
  pinMode(AIR_SENSOR_PIN, INPUT);
}

void loop() {
  // Read water level sensor
  int waterLevel = digitalRead(WATER_SENSOR_PIN);

  // Read air quality sensor
  int airQuality = analogRead(AIR_SENSOR_PIN);

  // Print sensor readings in real-time
  Serial.print("Water Level: ");
  Serial.println(waterLevel);

  Serial.print("Air Quality: ");
  Serial.println(airQuality);

  // Add your logic and actions based on sensor readings here

  // Optional delay for slower reading updates (adjust as needed)
  delay(1000);
}
