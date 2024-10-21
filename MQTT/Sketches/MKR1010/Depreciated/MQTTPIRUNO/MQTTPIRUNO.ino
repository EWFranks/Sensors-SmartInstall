#include <ArduinoMqttClient.h>
#include <WiFiNINA.h>
#include "arduino_secrets.h"
#include "config.h"

#define pirPin A0

const char deviceID[] = DEVICE_ID;
char ipStr[] = BROKER_IP;

unsigned long previousMillis = 0;
const long interval = 1000;  // Changed interval to 1000 milliseconds (1 second)

char ssid[] = SECRET_SSID;
char pass[] = SECRET_PASS;

int status = WL_IDLE_STATUS;
WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);

IPAddress brokerIP;
int port = 1883;

char topicMotion[] = "motion";


const int motionThreshold = 400; // Adjust this threshold based on your PIR sensor's sensitivity



void setup() {

  Serial.begin(9600);


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

  if (currentMillis - previousMillis >= interval) {

    previousMillis = currentMillis;

    String motionString;

    if (motion > motionThreshold) {
      motionString = "Motion detected";
    } else {
      motionString = "No motion";
    }

    SendMessage(topicMotion, motionString, "");

    Serial.println();
  }
}

void SendMessage(char topic[], String value, char unit[]) {
  Serial.print("Sending message to topic: ");
  Serial.println(topic);
  Serial.println(value);


  mqttClient.beginMessage(String(topic) + "/" + deviceID);
  mqttClient.print(value + " " + String(unit));
  mqttClient.endMessage();
}

void networkConnect() {
  while (status != WL_CONNECTED) {
    Serial.println("Attempting to connect to network: ");
    Serial.println(ssid);

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

  while (!mqttClient.connect(brokerIP, port)) {
    Serial.print("MQTT connection failed! Error code = ");
    Serial.println(mqttClient.connectError());
    networkConnect();
  }

  Serial.println("You're connected to the MQTT broker!");
  Serial.println();
}


