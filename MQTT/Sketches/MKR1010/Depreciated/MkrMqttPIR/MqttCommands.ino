
void onCommandMessage(int messageSize) {

  String messageTopic = mqttClient.messageTopic();
  String message = "";

  while(mqttClient.available())
    message += (char)mqttClient.read();

  String command = messageTopic.substring(14);
  Serial.println("command received: " + command);

  if(command == "restart") restartBoard();
  else if(command == "jingle") jinglePositive();

}


void restartBoard() {
  jingleNegative();
  NVIC_SystemReset();
}

void jinglePositive() {
  carrier.Buzzer.beep(600, 200);
  carrier.Buzzer.beep(400, 200);
  carrier.Buzzer.beep(800, 200);
}

void jingleNegative() {
  carrier.Buzzer.beep(800, 200);
  carrier.Buzzer.beep(400, 200);
  carrier.Buzzer.beep(200, 200);
}




void getMetadata() {

  // Get ID, Version, Directory
  
  deviceID = getID();
  directory = getOtaDirectory();
  version = getVersion();
  

  Serial.println("ID: " + deviceID);
  Serial.println("File Version: " + String(version) );
  Serial.println("OTA Directory: " + directory);

}


String getID() {
  // Find ID on SD card. If cannot find the ID, save the ID found in the config to the SD card for later

  String curID = "";

  if(SD.exists("ID.txt")) { // If file containing current ID exists on SD Card, read it
    
    Serial.println("Reading ID from SD Card...");
    File idFile = SD.open("id.txt", FILE_READ);

    while(idFile.available())
      curID += char(idFile.read());  // Get device ID from file
      
    idFile.close();

  } else { // If no file exists, create it and assign id from config.h
      setID(DEVICE_ID); // Set ID from Config
      curID = DEVICE_ID;
  }

  return curID;

}



void setID(char newID[]) {

    Serial.println("Writing ID to SD Card...");
    File idFile = SD.open("id.txt", FILE_WRITE); // Create file

    if(idFile) { // If the file opened:
      idFile.write(newID);  // Write config's id to file

    } else {
      Serial.println("Unable to write ID to SD Card!");
    }

    idFile.close();

}


int getVersion() {
    // Find version on SD card. If cannot find the ID, save the ID found in the config 0 (Guarantee Update).

    String versionString = "";

  if(SD.exists("VER.txt")) { // If file containing current ID exists on SD Card, read it
    Serial.println("Reading version from SD Card...");

    File versionFile = SD.open("ver.txt", FILE_READ);
    while(versionFile.available())
      versionString += char(versionFile.read());  // Get device ID from file

    versionFile.close();

  } else {
    return 0;  
  }
  

  return versionString.toInt();

}

void setVersion(int version) {

    Serial.println("Writing version to SD Card...");

    if(SD.exists("VER.txt"))
      SD.remove("VER.txt");

    File versionFile = SD.open("VER.txt", FILE_WRITE); // Create file

    if(versionFile) { // If the file opened:
      char versionChar[10];
      itoa(version, versionChar, 10);

      versionFile.write(versionChar);  // Write config's id to file

    } else {
      Serial.println("Unable to write version to SD Card!");
    }

    versionFile.close();

}





String getOtaDirectory() {
  // Find OtaDirectory on SD card. If cannot find the directory string, save the default string found in the config to the SD card for later

  String curDirectory = "";

  if(SD.exists("DIR.txt")) { // If file containing current ID exists on SD Card, read it
    Serial.println("Reading directory from SD Card...");

    File directoryFile = SD.open("DIR.txt", FILE_READ);

    while(directoryFile.available())
      curDirectory += char(directoryFile.read());  // Get device ID from file
      
    directoryFile.close();

  } else { // If no file exists, create it and assign id from config.h
      setOtaDirectory(SERVER_DIRECTORY); // Set ID from Config
      curDirectory = SERVER_DIRECTORY;
  }

  return curDirectory;

}



void setOtaDirectory(char newDirectory[]) {

    Serial.println("Writing Directory to SD Card...");
    File directoryFile = SD.open("DIR.txt", FILE_WRITE); // Create file

    if(directoryFile) {
      directoryFile.write(newDirectory);  // Write config's id to file      
      directoryFile.close();

    } else {
      Serial.println("Unable to write directory to SD Card!");
    }

}



void handleCommands() {

}


// char* charify(String aString) {

//   int length = aString.length();
//   char buffer[length];

//   aString.toCharArray(buffer, length);
//   return buffer;

// }




