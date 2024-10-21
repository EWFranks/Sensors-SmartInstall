# MqttHeartbeat

## Purpose

The purpose of this Arduino sketch is to simply send an empty heartbeat message through the MQTT network when powered.

## Overview

This script is a modified version of *MkrMqttPublisher.ino*, designed to be used to monitor certain device that may not have MQTT capabilities.

This use case is reliant on the monitored device having a USB port that is powered whenever the device is on. Connecting a WiFi-enabled Arduino running this sketch into that USB port will cause a heartbeat message to be sent through whenever the monitored device is on, and no messages to be sent whenever the device is off.
