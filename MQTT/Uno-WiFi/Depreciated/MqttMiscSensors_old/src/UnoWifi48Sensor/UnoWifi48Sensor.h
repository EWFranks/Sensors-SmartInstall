#ifndef UNO_WIFI_48_SENSOR_H
#define UNO_WIFI_48_SENSOR_H

#include <Arduino.h>

int ultrasonic(int inputPin, int outputPin);
bool flame(int inputPin);
int sound();
int water(int inputPin);
int gas(int inputPin);
int vibration(int inputPin);

#endif