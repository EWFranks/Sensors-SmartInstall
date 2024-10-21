
#include <Arduino_MKRIoTCarrier.h>
#include <ArduinoJson.h>

#include "config.h"

#include "src/SDConfig/SDConfig.h"


void setup() {
  // put your setup code here, to run once:

  Config config("config.txt");

  
  Serial.begin(9600);
  delay(2000);
  Serial.println("Ready:");

  carrierSetup();

  if(!config.loadOptions()) {
    Serial.println("Options cannot be loaded! Creating new options!");

    strlcpy(config.options.device_id, DEVICE_ID, sizeof(config.options.device_id));
    strlcpy(config.options.update_file, UPDATE_FILE, sizeof(config.options.update_file));

    config.saveOptions();

  } else Serial.println("Options loaded!");

  config.printOptions();  
    

}

void loop() {
}


MKRIoTCarrier carrier;

/**
 * @brief Initializes the necessary settings for the MKR IoT Carrier Rev2.
 * 
 */
void carrierSetup() {

  Serial.println("Carrier Setting Up:");

  carrier.noCase();

  if(!carrier.begin()) {
    Serial.println("Carrier not connected, check connections");
  }

  carrier.Light.begin();
  carrier.Light.setGestureSensitivity(100);

}
