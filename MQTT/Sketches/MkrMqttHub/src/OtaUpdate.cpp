/*
  Modified from OTASketchDownloadSD.ino by Nicola Elia for the ARduinoOTA library
  by Sterling Fisher
*/


#include "headers/OtaUpdate.h"

#include "libraries/SDU/src/SDU.h"


int serverVersion = -1; // Stores the version number retrieved from the server




/**
 * @brief Checks for an update on the server, and if there is one, downloads it to the SD card.
 * 
 * @param wifiClient The WiFiClient object that is used to connect to the server.
 * @param configPointer A pointer to the configPointer object
 * @param server The server IP address as a char array
 * @param server_port The server port number
 * 
 */
void checkUpdate(WiFiClient wifiClient, Config* configPointer, char server[], int server_port) {

  HttpClient serverClient(wifiClient, server, server_port);


  Serial.println("Check for update file: " + String(configPointer->options.update_file));

  if(!updateAvailable(serverClient, configPointer)) {
    Serial.println("OTA has no new updates available.");
    serverClient.stop();
    return;
  }

  Serial.println("OTA has new update available.");
  handleUpdate(serverClient, configPointer);

}


/**
 * @brief Updates the sketch on the board immediately, without checking for a new version.
 * 
 * @param wifiClient The WiFiClient object that is used to connect to the server.
 * @param configPointer A pointer to the configPointer object
 * @param server The server IP address as a char array
 * @param server_port The server port number
 * 
 */
void forceUpdate(WiFiClient wifiClient, Config* configPointer, char server[], int server_port) {

  HttpClient serverClient(wifiClient, server, server_port);
  handleUpdate(serverClient, configPointer);

}


/**
 * @brief Handles the update process. Downloads the update file to the SD card, and then resets the board.
 * 
 * @param serverClient The HttpClient object that is used to connect to the server.
 * @param configPointer A pointer to the configPointer object
 * 
 */
void handleUpdate(HttpClient serverClient, Config* configPointer) {
  
  String uploadPath = "/api/uploads/" + String(configPointer->options.update_file);
  serverClient.get(uploadPath); // Make the GET request for the OTA update file
  

  int statusCode = serverClient.responseStatusCode();
  Serial.print("Update status code: ");
  Serial.println(statusCode);

  if (statusCode != 200) { // If the server did not return 200 OK, stop update.
    Serial.println("Unable to get file.");
    serverClient.stop();
    return;
  }


  long length = serverClient.contentLength();
  if (length == HttpClient::kNoContentLengthHeader) {
    serverClient.stop();
    Serial.println("Server didn't provide Content-length header. Can't continue with update.");
    return;
  }


  Serial.println("Server returned update file of size " + String(length) + " bytes");

    if(!buildUpdate(serverClient, length)) // Build update file onto SD card from bytes
      return;


  configPointer->options.version = serverVersion;
  configPointer->saveOptions();

  Serial.println("Update file saved. Resetting...");
  NVIC_SystemReset();
}


/**
 * @brief Builds the update file onto the SD card from the bytes received from the server.
 * 
 * @param serverClient The HttpClient object that receives the bytes from the server.
 * @param length The length of the file to be received.
 * @return true if the file was successfully built.
 * @return false if the building process failed.
 * 
 */
bool buildUpdate(HttpClient serverClient, long length) {
  File file = SD.open(BIN_FILENAME, O_CREAT | O_WRITE);

  // If the file can't be created, stop update.
  if (!file) {
    Serial.println("Update file unable to be created.");
    serverClient.stop();
    return false;
  }

  // Bit of code to get the bytes from the server and write it into file 
  byte b;
  while (length > 0) {
    if (!serverClient.readBytes(&b, 1)) // reading a byte with timeout
      break;
    file.write(b); // writing a byte to the file
    length--;
  }

  // File and server connection cleanup
  file.close();
  serverClient.stop();
  
  // If the file was not fully downloaded, delete file.
  if (length > 0) {
    SD.remove(BIN_FILENAME);

    Serial.print("Timeout downloading update file at ");
    Serial.print(length);
    Serial.println(" bytes. Can't continue with update.");
    
    return false;
  }

  return true;
}




/**
 * @brief Checks to see if there is a new version available on the server.
 * 
 * @return if true, proceed with the update.
 * @return if false, Cancel update, continue with sketch.
 */
bool updateAvailable(HttpClient client, Config* configPointer) {

  //ScreenPrint("Sketch Version: " + String(version));
  
  String versionPath = "/uploads/version/" + String(configPointer->options.update_file);
  Serial.println(versionPath);

  client.get(versionPath); // Make the GET request for the version
  
  if(client.responseStatusCode() != 200) {
    Serial.println("Unable to get version.");
    return false; // If we can't get version, stop update.
  }

    String serverVersionString = client.responseBody();
    serverVersion = serverVersionString.toInt(); // Convert version to int
    //newVersion = serverVersion; // If we can get server version, the new version written 
    
    Serial.println("Server version found: " + serverVersionString);
    return (configPointer->options.version < serverVersion); // If we have latest version, return false and stop update.

}