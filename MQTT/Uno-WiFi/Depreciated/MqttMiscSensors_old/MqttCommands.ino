
void onCommandMessage(int messageSize) {

  String messageTopic = mqttClient.messageTopic();
  String message = "";

  while(mqttClient.available())
    message += (char)mqttClient.read();

  String command = messageTopic.substring(14);
  Serial.println("command received: " + command);

}



void handleCommands() {

}


// char* charify(String aString) {

//   int length = aString.length();
//   char buffer[length];

//   aString.toCharArray(buffer, length);
//   return buffer;

// }




