# bluetoothAdvertise

## Purpose

The purpose of this Arduino sketch is to allow a device to enable its BLE functionality to help test BLE triangulation methods.

## Overview

This script simply enables BLE on the host device, so that it can shows up on the scanner. It advertises nothing but the initial *Advertisement Data* of the device, and as such only the data from the connection itself is relevant.

To utilize with triangulation, a host computer can read the signal strength (RSSI) of the connection and utilize that to guess the distance between the host computer and the BLE device.

This script is designed to be simple enough to run on the flash of an Uno WiFi Rev2, but can also run on a Nano 33 BLE Sense or a MKR WiFi 1010.
