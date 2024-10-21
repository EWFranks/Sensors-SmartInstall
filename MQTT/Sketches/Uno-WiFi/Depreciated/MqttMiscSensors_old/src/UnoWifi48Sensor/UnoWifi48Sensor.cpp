#include <Arduino.h>
// int inputPin=4; // define ultrasonic signal receiver pin ECHO to D4 
// int outputPin=5; // define ultrasonic signal transmitter pin TRIG to D5
// int waterPin = 1; //connect water sensor to analog interface 0
// int gasPin = 2; //connect water sensor to analog interface 0
// int vibPin = 3; //connect water sensor to analog interface 0
// const int flamePin = 3;     // the number of the flame pin (D3)
// int State = 0;         // variable for reading status

int prevFlameState = 0;    // variable for switching flame reading


/**
 * @brief Returns the distance of the ultrasonic sensor in cm.
 * 
 * @param inputPin The Echo pin of the ultrasonic sensor
 * @param outputPin The Trig pin of the ultrasonic sensor
 * @return distance if object detected, -1 otherwise 
 */
int ultrasonic(int inputPin, int outputPin) {

  pinMode(inputPin, INPUT);
  pinMode(outputPin, OUTPUT); // Set output pin for ultrasonic sensor
  
  digitalWrite(outputPin, LOW); 
  delayMicroseconds(2);

  digitalWrite(outputPin, HIGH); // Pulse for 10μ s to trigger ultrasonic detection

  delayMicroseconds(10); 
  digitalWrite(outputPin, LOW);

  int distance = pulseIn(inputPin, HIGH); // Read receiver pulse time 
  distance= distance/58; // Transform pulse time to distance

  if(distance >= 300) { // No object detected
    return -1;
  }

  return distance;

}



bool flame(int inputPin) {

  int State = digitalRead(inputPin);
  
  if (State == HIGH && State != prevFlameState) {     
    // Send Flame on message 
    prevFlameState = HIGH;
    return true;
  } 
  else if(State == LOW && State != prevFlameState) {
    // Send flame off message 
    prevFlameState = LOW;
    // continue function, returning false
  }

  return false;
}



int sound() {
  int val = analogRead(0);   //connect mic sensor to Analog 0 
  return val;
}



int water(int inputPin) {
  int level;
  level = analogRead(inputPin); 
  return map(level, 0, 1023, 0, 100);
}



int gas(int inputPin) {
  
  int gasReading = analogRead(inputPin);
  return gasReading;

}



int vibration(int inputPin) {
  int vib = analogRead(inputPin);
  
  return map(vib, 0, 1023, 0, 100);;
}




// void setup()

// {

//   Serial.begin(9600); 
//   pinMode(inputPin, INPUT); 
//   pinMode(outputPin, OUTPUT);
//   // initialize the pushbutton pin as an input:
//   pinMode(flamePin, INPUT);

// }

// void loop()

// {

//   Serial.println("Distance: "+String(ultrasonic())); //Output distance
//   Serial.println("Sound: "+String(sound()));
//   Serial.println("Water Level: "+String(water()));
//   Serial.println("Gas: "+String(gas()));
//   Serial.println("Vibration: "+String(vibration()));
//   delay(500);
// }