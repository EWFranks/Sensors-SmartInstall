#ifndef OTAUPDATE_H
#define OTAUPDATE_H

#include <Arduino.h>
#include <ArduinoHttpClient.h>
#include <WiFiNINA.h>

#include "SdConfig.h"


#define BIN_FILENAME "UPDATE.BIN"


void checkUpdate(WiFiClient wifiClient, Config* configPointer, char server[], int server_port);
void forceUpdate(WiFiClient wifiClient, Config* configPointer, char server[], int server_port);
void handleUpdate(HttpClient serverClient, Config* configPointer);


bool buildUpdate(HttpClient serverClient, long length);
bool updateAvailable(HttpClient client, Config* config);


#endif