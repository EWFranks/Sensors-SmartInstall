
// Author: Sterling Fisher
// Date: 8/2/23
// Purpose: Button, then send post

#include <WiFiNINA.h>
#include <ArduinoHttpClient.h>

int buttonPin = 0;
int prevVal = -1;
int val = -1;

#include "arduino_secrets.h"
#include "config.h"

  // Enter server info in config files
char server[] = SERVER_IP;
unsigned short port = SERVER_PORT;
char path[] = PATH;       // Set the URI to the .bin firmware
unsigned long CHECK_INTERVAL = 6000;  // Time interval between update checks (ms)

  // Enter credentials in arduino_secrets.h
char ssid[] = SECRET_SSID;
char pass[] = SECRET_PASS;



WiFiClient wifiClient;
int status = WL_IDLE_STATUS; // Wifi radio's status

HttpClient client(wifiClient, server, port);





void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(buttonPin, INPUT);

  networkConnect();
  client.setTimeout(5);
  client.setHttpResponseTimeout(5);

}

void loop() {
  // put your main code here, to run repeatedly:
  prevVal = val;
  val = digitalRead(buttonPin);

  if(val < prevVal) {
    Serial.println("Button pressed!");
    sendNotification();
  }
}




void networkConnect() {
  // Connect to WiFi

  while(status != WL_CONNECTED) { // While not connected
      Serial.println("Attempting to connect to network: ");
      Serial.println(ssid);

      // FOR WPA2 CONNECTION:
      status = WiFi.begin(ssid, pass); // Connect to WPA/WPA2 network
      delay(5000); // Wait 15 seconds for connection
    }

// When connected:  
  Serial.println("You're connected to the network! Yay!");
  IPAddress ip = WiFi.localIP();
  Serial.println(ip);

}



void sendNotification() {
// Post W/ Headers
  // Serial.println("making POST request");
  // String postData = "";

  // client.beginRequest();
  // client.post(path);
  // client.sendHeader("Content-Type", "application/x-www-form-urlencoded");
  // client.sendHeader("Content-Length", postData.length());
  // client.sendHeader("X-Custom-Header", "custom-header-value");
  // client.beginBody();
  // client.print(postData);
  // client.endRequest();

  Serial.println("making POST request");
  String contentType = "application/x-www-form-urlencoded";
  String postData = "BUTTON";

  client.post("/", contentType, postData);

  // read the status code and body of the response
  int statusCode = client.responseStatusCode();
  String response = client.responseBody();

  Serial.println("Status code: " + String( client.responseStatusCode() ));
  Serial.println("Response: " + response);

}
