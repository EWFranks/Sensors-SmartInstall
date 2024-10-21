
#include <ArduinoMqttClient.h>
#include <WiFiNINA.h>

#include "Arduino_SensorKit.h"
#include <Adafruit_AHTX0.h>

#include "arduino_secrets.h"
#include "config.h"

  // SENSOR VARIABLES
Adafruit_AHTX0 aht;

int sound_sensor = A2;
int light_sensor = A3;

String deviceID = DEVICE_ID;

  // CONFIG VARIABLES
char ipStr[] = BROKER_IP;

// Enter credentials in arduino_secrets.h
char ssid[] = SECRET_SSID;
char pass[] = SECRET_PASS;

int status = WL_IDLE_STATUS; // Wifi radio's status

WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);

IPAddress brokerIP;
int port = 1883;


  // MQTT VARIABLES
//Interval for sending messages in milliseconds
const long interval = 8000;
unsigned long previousMillis = 0;


void networkConnect() {

  while(status != WL_CONNECTED) { // While not connected
      Serial.println("Attempting to connect to network: ");
      Serial.println(ssid);

      // FOR WPA2 CONNECTION:
      status = WiFi.begin(ssid, pass); // Connect to WPA/WPA2 network
      delay(15000); // Wait 15 seconds for connection
    }

// When connected:  
  Serial.println("You're connected to the network! Yay!");
  IPAddress ip = WiFi.localIP();
  Serial.println(ip);

}



void brokerConnect() {

  brokerIP.fromString(ipStr);

  Serial.print("Attempting to connect to the MQTT broker: ");
  Serial.println(brokerIP);
  
  if(!mqttClient.connect(brokerIP, port)) {
    Serial.print("MQTT connection failed! Error code = ");
    Serial.println(mqttClient.connectError());
      
    while(1); // End program

   }

    Serial.println("You're connected to the MQTT broker!");
    Serial.println();
}



void setup() {

  Serial.begin(9600);
  while(!Serial); // Stall if can't connect to Serial

  networkConnect();
  brokerConnect();



// Initialization
  //Environment.begin();
  aht.begin();
  Pressure.begin();
  Accelerometer.begin();
  


}


void SendMessage(char topic[], String value, char unit[]) {
  Serial.print("Sending message to topic: ");
  Serial.println(topic);
  Serial.println(value + " " + unit);

  mqttClient.beginMessage(String( topic ) + "/" + deviceID);
  mqttClient.print(value + " " + String( unit )); // Why oh why did I do char[] for some but not others
  mqttClient.endMessage();

}

void loop() {

  //Serial.print(String( Environment.readTemperature() ) + " ");
  //Serial.print(String( Environment.readHumidity() ) + " ");

  unsigned long currentMillis = millis();

  if(currentMillis - previousMillis >= interval) {
    sensors_event_t humidity, temp;
    aht.getEvent(&humidity, &temp);

    // Serial.print("temperature:" + String( temp.temperature ) + " ");
    SendMessage("temperature", String( temp.temperature ), "celsius");

    // Serial.print("humidity:" + String( humidity.relative_humidity ) + " ");
    SendMessage("humidity", String( humidity.relative_humidity ), "percent");

    // Serial.print("pressure:" + String( Pressure.readPressure()/100 ) + "hectopascal");
    SendMessage("humidity", String( Pressure.readPressure()/100 ), "percent");

    // Serial.print("altitude:" + String(Pressure.readAltitude() ) + "meters");
    SendMessage("altitude", String( Pressure.readAltitude() ), "meters");
    
    // Serial.print("light:" + String( lightSensor() ) + " ");
    SendMessage("light", String( lightSensor() ), "percent");

    // Serial.print("sound:" + String( soundSensor() ) + " ");
    SendMessage("sound", String( soundSensor() ), "percent");

    String acceleration = String( Accelerometer.readX() ) + "_" + String( Accelerometer.readY() ) + "_" + String( Accelerometer.readZ() );
    // Serial.print("acceleration:" + acceleration + " ");
    SendMessage("acceleration", acceleration, "delta");


    delay(2000); // Delay 2 seconds

  }




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
