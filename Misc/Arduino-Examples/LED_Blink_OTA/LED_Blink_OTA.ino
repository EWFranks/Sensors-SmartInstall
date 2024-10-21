
// Combined WiFi101_OTA example from ArduinoOTA library and LED_Blink example from MKRIOTCarrier library



#include <Arduino_MKRIoTCarrier.h>
MKRIoTCarrier carrier;

#include <SPI.h>
//#include <SD.h>
#include <WiFiNINA.h>
#include <ArduinoOTA.h>
//#include <SDU.h>



#define NUMPIXELS 5
#define SPEED 2000

#include "arduino_secrets.h" 
///////please enter your sensitive data in the Secret tab/arduino_secrets.h
/////// Wifi Settings ///////
char ssid[] = SECRET_SSID;      // your network SSID (name)
char pass[] = SECRET_PASS;   // your network password

int status = WL_IDLE_STATUS;

void setup() {
  carrier.noCase();
  carrier.begin();

  Serial.begin(9600);

  // check for the presence of the shield:
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    // don't continue:
    while (true);
  }

  // attempt to connect to Wifi network:
  while ( status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);
  }

  // start the WiFi OTA library with internal (flash) based storage
  ArduinoOTA.begin(WiFi.localIP(), "Arduino", "password", InternalStorage);


}

// Runs 10 LEDs at a time along strip, cycling through red, green and blue.
// This requires about 200 mA for all the 'on' pixels + 1 mA per 'off' pixel.

int      head  = 0, tail = -4; // Index of first 'on' and 'off' pixels
uint32_t color = 0xFF0000;      // 'On' color (starts red)

void loop() {

  ArduinoOTA.poll();

  carrier.leds.setPixelColor(head, color); // 'On' pixel at head
  carrier.leds.setPixelColor(tail, 0);     // 'Off' pixel at tail
  carrier.leds.show();                     // Refresh strip
  delay(SPEED);                        // Pause 20 milliseconds (~50 FPS)


  if (++head >= NUMPIXELS) {        // Increment head index.  Off end of strip?
    head = 0;                       //  Yes, reset head index to start
    if ((color >>= 8) == 0)         //  Next color (R->G->B) ... past blue now?
      color = 0xFF0000;             //   Yes, reset to red
  }
  if (++tail >= NUMPIXELS) tail = 0; // Increment, reset tail index
}
