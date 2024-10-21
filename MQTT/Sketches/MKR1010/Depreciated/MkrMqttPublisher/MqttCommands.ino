
bool carrierEnabled = true;
bool PirEnabled = false;



  /**
 * @brief Callback function for receiving commands through MQTT.
 * 
 * @param messageSize The size of the received message. Handled by the MQTT library.
 * 
 */
void onCommandMessage(int messageSize) {

  String messageTopic = mqttClient.messageTopic();
  String message = "";

  while (mqttClient.available())
    message += (char)mqttClient.read();

  String command = messageTopic.substring(14);
  Serial.println("command received: " + command);

  if (command == "restart") restartBoard();
  else if (command == "jingle") jinglePositive();
  else if (command == "check_update") handleSketchDownload();
  else if (command == "force_update") forceUpdate();

  else if (command == "set/id") setIdCommand(message);
  else if (command == "set/directory") setDirectoryCommand(message);
}



/**
 * @brief Restarts the board.
 * Plays a little jingle as well.
 * 
 */
void restartBoard() {
  jingleNegative();
  NVIC_SystemReset();
}

/**
 * @brief Plays an optimistic jingle on the carrier buzzer.  
 * 
 */
void jinglePositive() {
  carrier.Buzzer.beep(600, 200);
  carrier.Buzzer.beep(400, 200);
  carrier.Buzzer.beep(800, 200);
}

/**
 * @brief Plays a pessimistic jingle on the carrier buzzer.
 * 
 */
void jingleNegative() {
  carrier.Buzzer.beep(800, 200);
  carrier.Buzzer.beep(400, 200);
  carrier.Buzzer.beep(200, 200);
}

/**
 * @brief Forces the sketch to update.
 * Grabs whatever the latest version of the sketch is on the server.
 * 
 */
void forceUpdate() {
  version = 0;
  handleSketchDownload();
}

/**
 * @brief Sets the ID of the device, storing it on the SD card.
 * 
 * @param message the new ID of the board.
 */
void setIdCommand(String message) {
  char out[32];
  message.toCharArray(out, 32);

  setID(out);
  restartBoard();
}

/**
 * @brief Sets the OTA directory of the device, storing it on the SD card.
 * 
 * @param message the new OTA directory of the board.
 */
void setDirectoryCommand(String message) {
  char out[64];
  message.toCharArray(out, 64);

  setOtaDirectory(out);
  restartBoard();
}


/**
 * @brief Gets the data stored on the SD card.
 * Stores the data in global variables.
 * 
 */
void getMetadata() {

  // Get ID, Version, Directory

  deviceID = getID();
  directory = getOtaDirectory();

  if (!VERSION_RESET)
    version = getVersion();
  else {
    version = 0;
    setVersion(0);
  }


  Serial.println("ID: " + deviceID);
  Serial.println("File Version: " + String(version));
  Serial.println("OTA Directory: " + directory);
}

/**
 * @brief Gets the ID of the device from the SD card.
 * If not on the SD card, pulls the ID from the config and saves it to the SD card.
 * 
 * @return String 
 */
String getID() {
  // Find ID on SD card. If cannot find the ID, save the ID found in the config to the SD card for later

  String curID = "";

  if (!ID_RESET && SD.exists("ID.txt")) {  // If file containing current ID exists on SD Card, read it

    Serial.println("Reading ID from SD Card...");
    File idFile = SD.open("id.txt", FILE_READ);

    while (idFile.available())
      curID += char(idFile.read());  // Get device ID from file

    idFile.close();

  } else {             // If no file exists, create it and assign id from config.h
    setID(DEVICE_ID);  // Set ID from Config
    curID = DEVICE_ID;
  }

  return curID;
}


/**
 * @brief Sets the ID of the device, storing it on the SD card.
 * 
 * @param newID The new ID of the device.
 */
void setID(char newID[]) {

  Serial.println("Writing ID to SD Card...");

  if (SD.exists("ID.txt"))
    SD.remove("ID.txt");

  File idFile = SD.open("id.txt", FILE_WRITE);  // Create file

  if (idFile) {           // If the file opened:
    idFile.write(newID);  // Write config's id to file

  } else {
    Serial.println("Unable to write ID to SD Card!");
  }

  idFile.close();
}

/**
 * @brief Gets the version of the device from the SD card.
 * If not on the SD card, pulls the version from the config and saves it to the SD card.
 * 
 * @return int
 */
int getVersion() {
  // Find version on SD card. If cannot find the ID, save the ID found in the config 0 (Guarantee Update).

  String versionString = "";

  if (SD.exists("VER.txt")) {  // If file containing current ID exists on SD Card, read it
    Serial.println("Reading version from SD Card...");

    File versionFile = SD.open("ver.txt", FILE_READ);
    while (versionFile.available())
      versionString += char(versionFile.read());  // Get device ID from file

    versionFile.close();

  } else {
    return 0;
  }


  return versionString.toInt();
}

/**
 * @brief Sets the version of the device, storing it on the SD card.
 * 
 * @param version The new version of the device.
 */
void setVersion(int version) {

  Serial.println("Writing version to SD Card...");

  if (SD.exists("VER.txt"))
    SD.remove("VER.txt");

  File versionFile = SD.open("VER.txt", FILE_WRITE);  // Create file

  if (versionFile) {  // If the file opened:
    char versionChar[10];
    itoa(version, versionChar, 10);

    versionFile.write(versionChar);  // Write config's id to file

  } else {
    Serial.println("Unable to write version to SD Card!");
  }

  versionFile.close();
}




/**
 * @brief Gets the OTA directory of the device from the SD card.
 * If not on the SD card, pulls the directory from the config and saves it to the SD card.
 * 
 * @return String 
 */
String getOtaDirectory() {
  // Find OtaDirectory on SD card. If cannot find the directory string, save the default string found in the config to the SD card for later

  String curDirectory = "";

  if (!DIRECTORY_RESET && SD.exists("DIR.txt")) {  // If file containing current ID exists on SD Card, read it
    Serial.println("Reading directory from SD Card...");

    File directoryFile = SD.open("DIR.txt", FILE_READ);

    while (directoryFile.available())
      curDirectory += char(directoryFile.read());  // Get device ID from file

    directoryFile.close();

  } else {                              // If no file exists, create it and assign id from config.h
    setOtaDirectory(SERVER_DIRECTORY);  // Set ID from Config
    curDirectory = SERVER_DIRECTORY;
  }

  return curDirectory;
}


/**
 * @brief Sets the OTA directory of the device, storing it on the SD card.
 * 
 * @param newDirectory The new OTA directory of the device.
 */
void setOtaDirectory(char newDirectory[]) {

  Serial.println("Writing Directory to SD Card...");

  if (SD.exists("DIR.txt"))
    SD.remove("DIR.txt");

  File directoryFile = SD.open("DIR.txt", FILE_WRITE);  // Create file

  if (directoryFile) {
    directoryFile.write(newDirectory);  // Write config's id to file
    directoryFile.close();

  } else {
    Serial.println("Unable to write directory to SD Card!");
  }
}


// char* charify(String aString) {

//   int length = aString.length();
//   char buffer[length];

//   aString.toCharArray(buffer, length);
//   return buffer;

// }
