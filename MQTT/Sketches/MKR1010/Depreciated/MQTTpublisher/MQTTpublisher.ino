
#include <ArduinoMqttClient.h>
#include <Arduino_MKRIoTCarrier.h>
#include <WiFiNINA.h>

#include "arduino_secrets.h"
#include "config.h"

#include <ArduinoOTA.h>
#include <SDU.h>


String deviceID;

char ipStr[] = BROKER_IP;

// Enter credentials in arduino_secrets.h
char ssid[] = SECRET_SSID;
char pass[] = SECRET_PASS;


int status = WL_IDLE_STATUS; // Wifi radio's status

WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);

//const char broker[] = "test.mosquitto.org";
//IPAddress brokerIP(10,42,42,113);
//IPAddress brokerIP(10,0,2,154);
IPAddress brokerIP;
int port = 1883;

// Sending topics
char topicTemp[] = "temperature";
char topicMotion[] = "motion";
char topicHumidity[] = "humidity";
char topicPressure[] = "pressure";

char tempUnit[] = "celsius";
char humidityUnit[] = "percent";
char pressureUnit[] = "hectopascal";

  // MQTT VARIABLES
//Interval for sending messages in milliseconds
const long interval = 8000;
unsigned long previousMillis = 0;

int cursorX = 30;
int cursorY;

MKRIoTCarrier carrier;


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

void setID() {

  if(SD.exists("ID.txt")) {
    Serial.println("Reading ID from SD Card...");

    File idFile = SD.open("id.txt", FILE_READ);
    while(idFile.available())
    deviceID += char(idFile.read());  // Get device ID from file 
    
    //Serial.println(char(idFile.read()));
      
    idFile.close();

  } else { // If no file exists, create it and assign id from config.h
  
    Serial.println("Writing ID to SD Card...");

    File idFile = SD.open("id.txt", FILE_WRITE); // Create file

    Serial.println("Exists?: " + String(SD.exists("ID.txt")));

    if(idFile) {
      idFile.write(DEVICE_ID);  // Write config's id to file
      deviceID = DEVICE_ID;     // Set device id to config's id

      idFile.close();
    } else Serial.println("Unable to write ID to SD Card!");

  
  }

}



void setup() {

  carrier.noCase();

// PART 1: CONNECT TO WIFI

//Initialize serial communication and wait for port to open:
  Serial.begin(9600); // Attempt to connect to serial communication
  //while(!Serial); // Stall if not connected

  if(!carrier.begin()) {
    Serial.println("Carrier not connected, check connections");
    while(1);
  }

  DisplayHandler();

  networkConnect(); // Connect to Wifi

// PART 2: CONNECT TO BROKER

  brokerConnect(); // Connect to MQTT broker

  setID();


}

void DisplayHandler() {
  carrier.display.fillScreen(ST77XX_BLACK);

  cursorY = 80;
  carrier.display.setCursor(cursorX, cursorY);
  carrier.display.setTextColor(0xF800);
  carrier.display.setTextSize(2);
  carrier.display.print("Device ID: " + String(deviceID));

  carrier.display.setTextColor(ST77XX_WHITE);
  carrier.display.setTextSize(1);

  cursorY += 30;

}



void SendMessage(char topic[], String value, char unit[]) {
  Serial.print("Sending message to topic: ");
  Serial.println(topic);
  Serial.println(value);

  carrier.display.setCursor(cursorX, cursorY);
  //carrier.display.setTextColor(ST77XX_WHITE);
  carrier.display.print(String( topic ) + ": " + value + " " + String( unit ));
  cursorY += 10;

  mqttClient.beginMessage(String( topic ) + "/" + deviceID);
  mqttClient.print(value + " " + String( unit )); // Why oh why did I do char[] for some but not others
  mqttClient.endMessage();

}

void loop() {

  // call poll() regularly to allow the library to send MQTT keep alive which avoids being disconnected by the broker
  mqttClient.poll();

  // Check connection to Wifi and Broker
  if(WiFi.status() != WL_CONNECTED) {
    Serial.println("Wifi connection lost. Retrying...");

    networkConnect();
    brokerConnect();
  }

  if(!mqttClient.connected()) {
    Serial.println("MQTT connection lost. Retrying...");

    brokerConnect();
  }

  unsigned long currentMillis = millis();

  if(currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis; // Saves last time message was sent

    DisplayHandler();

    // recond random value from A0, A1, and A2
    //int Rvalue = analogRead(A0);
    float temp = carrier.Env.readTemperature();

    // TODO: IMPLEMENT REST
    //int motion = analogRead(A1);

    carrier.Light.setGestureSensitivity(100);

    uint8_t motion = -100;
    if(carrier.Light.gestureAvailable())
      motion = carrier.Light.readGesture();

    int humidity = carrier.Env.readHumidity();

    int pressure = carrier.Pressure.readPressure();
    
    SendMessage(topicTemp, String( temp ), tempUnit);

    String motionString = "None";

    if(motion == UP)
      motionString = "Up";
    else if(motion == DOWN)
      motionString = "Down";
    else if(motion == LEFT)
      motionString = "Left";
    else if(motion == RIGHT)
      motionString = "Right";

    SendMessage(topicMotion, motionString, "");
    
    // if(motion == -100)
    //   SendMessage(topicMotion, "None");
    // else
    //   SendMessage(topicMotion, String( motion ));
    
    
    SendMessage(topicHumidity, String( humidity ), humidityUnit);
    SendMessage(topicPressure, String( pressure ), pressureUnit);

    // Serial.print("Sending message to topic: ");
    // Serial.println(topicTemp);
    // Serial.println(temp);
    
    // Serial.print("Sending message to topic: "); // Random value motion
    // Serial.println(topicMotion);
    // Serial.println(motion);

    // Serial.print("Sending message to topic: "); // Random value air pressure
    // Serial.println(topicHumidity);
    // Serial.println(humidity);

    // Serial.print("Sending message to topic: ");
    // Serial.println(topicPressure);
    // Serial.println(pressure);

    // //Send message, the print interface can be used to set the message contents
    // mqttClient.beginMessage(topicTemp);
    // mqttClient.print(temp);
    // mqttClient.endMessage();

    // mqttClient.beginMessage(topicMotion);
    // mqttClient.print(motion);
    // mqttClient.endMessage();

    // mqttClient.beginMessage(topicHumidity);
    // mqttClient.print(humidity);
    // mqttClient.endMessage();

    // mqttClient.beginMessage(topicPressure);
    // mqttClient.print(pressure);
    // mqttClient.endMessage();

    Serial.println();



  }




}
