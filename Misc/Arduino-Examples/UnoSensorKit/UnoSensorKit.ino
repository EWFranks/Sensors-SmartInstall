
#include "Arduino_SensorKit.h"
#include <Adafruit_AHTX0.h>

Adafruit_AHTX0 aht;

int sound_sensor = A2;
int light_sensor = A3;

void setup() {

  Serial.begin(9600);
  while(!Serial); // Stall if can't connect to Serial

// Initialization
  //Environment.begin();
  aht.begin();
  Pressure.begin();
  Accelerometer.begin();
  


}

void loop() {

  //Serial.print(String( Environment.readTemperature() ) + " ");
  //Serial.print(String( Environment.readHumidity() ) + " ");

  sensors_event_t humidity, temp;
  aht.getEvent(&humidity, &temp);

  Serial.print("temperature:" + String( temp.temperature ) + " ");
  Serial.print("humidity:" + String( humidity.relative_humidity ) + " ");

  Serial.print("pressure:" + String( Pressure.readPressure() ) + " ");
  Serial.print("altitude:" + String(Pressure.readAltitude() ) + " ");
  
  Serial.print("light:" + String( lightSensor() ) + " ");
  Serial.print("sound:" + String( soundSensor() ) + " ");
  Serial.print("acceleration:" + String( Accelerometer.readX() ) + "_" + String( Accelerometer.readY() ) + "_" + String( Accelerometer.readZ() ) + " ");
  Serial.println();


  delay(2000); // Delay 2 seconds


}

int lightSensor() {
  
  int raw_light = analogRead(light_sensor);
  int light = map(raw_light, 0, 1023, 0, 100);

  return light;

}

int soundSensor() {
  int soundValue = 0;

  for(int i = 0; i < 32; i++) {
    soundValue += analogRead(sound_sensor); // Read sound sensor 32 times
  }

  soundValue >>= 5; // Shift 5 bits (uh sure)

  return soundValue;


}
