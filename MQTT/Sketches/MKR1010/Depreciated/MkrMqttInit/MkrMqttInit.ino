#include <WiFiNINA.h>
#include <Arduino_MKRIoTCarrier.h>

// Included DEFINES
  #include "arduino_secrets.h"
  #include "config.h"

// Set either on SD or in Config
  String deviceID;
  int version = 0;
  String directory = "";

// Enter credentials in arduino_secrets.h
  char ssid[] = SECRET_SSID;
  char pass[] = SECRET_PASS;


// WiFi Variables
  int status = WL_IDLE_STATUS; // Wifi radio's status

  WiFiClient wifiClient;

  MKRIoTCarrier carrier;





// ------ MAIN FUNCTIONS --------


void setup() {
  // Init function, runs before anything else

  //Initialize serial communication and wait for port to open:
  Serial.begin(9600); // Attempt to connect to serial communication
  
  carrierSetup(); // Setup Carrier Functions
  getMetadata(); // Read data from SD Card

  networkConnect(); // Connect to Wifi
  handleSketchDownload(); // Check for OTA update

}




// --------- SETUP FUNCTIONS --------


void carrierSetup() {
  // Initialize MKR IoT Carrier

  carrier.noCase();

  if(!carrier.begin()) {
    Serial.println("Carrier not connected, check connections");
    while(1);
  }
}



void networkConnect() {
  // Connect to WiFi

  while(status != WL_CONNECTED) { // While not connected
      Serial.println("Attempting to connect to network: ");
      Serial.println(ssid);

      // FOR WPA2 CONNECTION:
      status = WiFi.begin(ssid, pass); // Connect to WPA/WPA2 network
      delay(5000); // Wait 5 seconds for connection
    }

  // When connected:  
  Serial.println("You're connected to the network! Yay!");
  IPAddress ip = WiFi.localIP();
  Serial.println(ip);

}