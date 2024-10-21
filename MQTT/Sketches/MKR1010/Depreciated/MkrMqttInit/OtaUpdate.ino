/*
  Modified from OTASketchDownloadSD.ino by Nicola Elia for the ARduinoOTA library
  by Sterling Fisher
*/


#include <ArduinoHttpClient.h>
#include <ArduinoOTA.h>

//#include <SDU.h>
#include "src\SDU\src\SDU.h"


const char* BIN_FILENAME = "UPDATE.BIN";

// Please enter your WiFi sensitive data in the arduino_secrets.h file
#include "arduino_secrets.h"
#include "config.h"

//WiFiSSLClient wifiClientSSL;  // HTTPS

char server[] = SERVER_IP;
short server_port = SERVER_PORT;



HttpClient client(wifiClient, server, server_port);

int newVersion = -1;

void handleSketchDownload() {

  nextVersionSD();

}

bool nextVersionAvailable() {
  
  String versionPath = "/version/" + directory;
  Serial.println(versionPath);

  client.get(versionPath);
  
  if(client.responseStatusCode() == 200) {
    String serverVersionString = client.responseBody();
    int serverVersion = serverVersionString.toInt();
    newVersion = serverVersion; // If we can get server version, the new version written 
    
    Serial.println("Server version found: " + serverVersionString);
    return (version < serverVersion); // If we have latest version, stop update.

  }

  Serial.println("Server Version not found.");
  return true; // Continue update if not version available.

}

void nextVersionSD() {

  String uploadPath = "/uploads/" + directory;

  Serial.print("Check for update file ");
  Serial.println(directory);

  if(!nextVersionAvailable()) {
    Serial.println("No new version available.");
    client.stop();
    return;
  }


  // Make the GET request
  client.get(uploadPath);


  int statusCode = client.responseStatusCode();
  Serial.print("Update status code: ");
  Serial.println(statusCode);
  if (statusCode != 200) {
    Serial.println("Unable to get file.");
    client.stop();
    return;
  }

  long length = client.contentLength();
  if (length == HttpClient::kNoContentLengthHeader) {
    client.stop();
    Serial.println("Server didn't provide Content-length header. Can't continue with update.");
    return;
  }
  Serial.print("Server returned update file of size ");
  Serial.print(length);
  Serial.println(" bytes");

  File file = SD.open(BIN_FILENAME, O_CREAT | O_WRITE);

  if (!file) {
    client.stop();
    Serial.println("File has been found not. Continue. NOT");
    return;
  }
  byte b;
  while (length > 0) {
    if (!client.readBytes(&b, 1)) // reading a byte with timeout
      break;
    file.write(b);
    length--;
  }
  file.close();
  client.stop();
  if (length > 0) {
    SD.remove(BIN_FILENAME);
    Serial.print("Timeout downloading update file at ");
    Serial.print(length);
    Serial.println(" bytes. Can't continue with update.");
    return;
  }

  if(newVersion > 0)  // Set to server's version
    setVersion(newVersion);
  else
    setVersion(version+1);

  Serial.println("Update file saved. Resetting...");

  NVIC_SystemReset();

}
