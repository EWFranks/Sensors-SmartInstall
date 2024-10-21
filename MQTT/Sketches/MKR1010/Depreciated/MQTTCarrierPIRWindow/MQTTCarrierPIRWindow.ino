#include <ArduinoMqttClient.h>
#include <Arduino_MKRIoTCarrier.h>
#include <WiFiNINA.h>
#include "arduino_secrets.h"
#include "config.h"

#define pirPin A0

const char deviceID[] = DEVICE_ID;
char ipStr[] = BROKER_IP;

unsigned long previousMillis = 0;
const long interval = 250;  // Changed interval to 1000 milliseconds (1 second)

char ssid[] = SECRET_SSID;
char pass[] = SECRET_PASS;

int status = WL_IDLE_STATUS;
WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);

IPAddress brokerIP;
int port = 1883;

char topicTemp[] = "temperature";
char topicMotion[] = "motion";
char topicHumidity[] = "humidity";
char topicPressure[] = "pressure";

char tempUnit[] = "celsius";
char humidityUnit[] = "percent";
char pressureUnit[] = "hectopascal";

MKRIoTCarrier carrier;

const int motionThreshold = 200; // Adjust this threshold based on your PIR sensor's sensitivity
int previousMotion = 0;
int cursorX = 30;
int cursorY;


void DisplayHandler() {
  // Clear screen, set text settings, and write device ID

  carrier.display.fillScreen(ST77XX_BLACK); // Clear screen

  cursorY = 80; // Reset text vertical position to close to top of the screen
  carrier.display.setCursor(cursorX, cursorY); // Set text horizontal and vertical position

  // Write ID
  carrier.display.setTextColor(0xF800); // Set text color to red
  carrier.display.setTextSize(2);
  carrier.display.print("Device ID: " + String(deviceID));

  carrier.display.setTextColor(ST77XX_WHITE); // Change text color to white for readings
  carrier.display.setTextSize(1); // Shrink reading text

  cursorY += 30;  // Change text vertical position for next line

}

void ScreenPrint(String message) {
  carrier.display.fillScreen(ST77XX_BLACK); // Clear screen

  cursorY = 80; // Reset text vertical position to close to top of the screen
  carrier.display.setCursor(cursorX, cursorY); // Set text horizontal and vertical position

  // Write ID
  carrier.display.setTextColor(0xF800); // Set text color to red
  carrier.display.setTextSize(1);
  carrier.display.print(message);

  carrier.display.setTextColor(ST77XX_WHITE); // Change text color to white for readings
  carrier.display.setTextSize(1); // Shrink reading text

}


void setup() {
  CARRIER_CASE = false;

  Serial.begin(9600);

  if (!carrier.begin()) {
    Serial.println("Carrier not connected, check connections");
    while (1);
  }

  networkConnect();
  brokerConnect();
}

void loop() {
  
  mqttClient.poll();

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi connection lost. Retrying...");
    networkConnect();
    brokerConnect();
  }

  if (!mqttClient.connected()) {
    Serial.println("MQTT connection lost. Retrying...");
    brokerConnect();
  }

  unsigned long currentMillis = millis();
  int motion = analogRead(pirPin);  // Changed digitalRead to analogRead for PIR sensor

  if (motion > motionThreshold) {
    
    carrier.leds.fill(carrier.leds.Color(0, 200, 0), 0, 5);
    carrier.leds.show();
      
  }

  if (currentMillis - previousMillis >= interval) {
    
    DisplayHandler();
    previousMillis = currentMillis;
    
    //Serial.println("code zone 1");
    //float temp = carrier.Env.readTemperature();
    //Serial.println("code zone 2");
    //int humidity = carrier.Env.readHumidity();

    //int pressure = carrier.Pressure.readPressure();
    //Serial.println("code zone 3");
    //SendMessage(topicTemp, String(temp), tempUnit);
    
    String motionString;
    
    if (motion > motionThreshold) {
      if(previousMotion == 0){
        motionString = "Motion detected";
        SendMessage(topicMotion, motionString, "");
        carrier.leds.fill(carrier.leds.Color(0, 200, 0), 0, 5);
        previousMotion = 1;
      } 
    } else {
      if(previousMotion == 1){
        motionString = "No motion";
        carrier.leds.fill(carrier.leds.Color(0, 0, 200), 0, 5);
        SendMessage(topicMotion, motionString, "");
        previousMotion = 0;
      }
    }
    carrier.leds.show();

    

    //SendMessage(topicHumidity, String(humidity), humidityUnit);
    //SendMessage(topicPressure, String(pressure), pressureUnit);

  }
  
}

void SendMessage(char topic[], String value, char unit[]) {
  Serial.print("Sending message to topic: ");
  Serial.println(topic);
  Serial.println(value);

  carrier.display.setCursor(cursorX, cursorY);  // Set text horizontal (static) and vertical position
  carrier.display.print(String(topic) + ": " + value + " " + String(unit));
  cursorY += 10;  // Change text vertical position for next line

  mqttClient.beginMessage(String(topic) + "/" + deviceID);
  mqttClient.print(value + " " + String(unit));
  mqttClient.endMessage();
}

void networkConnect() {
  while (status != WL_CONNECTED) {
    Serial.println("Attempting to connect to network: ");
    Serial.println(ssid);

  ScreenPrint("Connecting: " + String(ssid));

    status = WiFi.begin(ssid, pass);
    delay(15000);
  }

  Serial.println("You're connected to the network! Yay!");
  IPAddress ip = WiFi.localIP();
  Serial.println(ip);
}

void brokerConnect() {
  brokerIP.fromString(ipStr);

  Serial.print("Attempting to connect to the MQTT broker: ");
  Serial.println(brokerIP);

  ScreenPrint("Connecting: " + String(ipStr));

  while (!mqttClient.connect(brokerIP, port)) {
    Serial.print("MQTT connection failed! Error code = ");
    Serial.println(mqttClient.connectError());
    networkConnect();
  }

  Serial.println("You're connected to the MQTT broker!");
  Serial.println();
}


