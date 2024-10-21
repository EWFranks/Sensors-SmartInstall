# Bleak Overview

Bleak is a Python library that allows us to connect to each BLE-enabled Sensor Hub and read the broadcasted services and their characteristics and data.

Each Python script utilizing Bleak must be ran on a BLE-enabled device, or the script will not be able to connect to any devices and will either end immediately or loop forever.
