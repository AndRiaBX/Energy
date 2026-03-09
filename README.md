ESP32 Smart Sensor System

IoT system using an ESP32 and Python server. 
ESP32 reads temperature, humidity, motion, light, and current, sends JSON to Python, and controls fan and LED.

Version 1: EspCodeV1 / PythonCodeV1
ESP32 connects to home WiFi, Python reads via TCP

Version 2: EspCodeV2 / PythonCodeV2
ESP32 hosts WiFi and HTML dashboard, Python reads from ESP32

Hardware: ESP32, DHT22, PIR, LDR, ACS712, relay, LED, resistors, wires

Pins: DHT22 → 26
PIR → 14
LDR → 35
ACS712 → 32
Relay → 25
LED → 4

Software: ESP32 libraries WiFi, WebServer, DHT, ArduinoJson
Python uses standard libraries: socket, json, datetime, time

Features: temperature, humidity, motion, light, power measurement
Fan and LED automation
Automatic relay off after inactivity in dark

Usage: Upload firmware, configure WiFi if needed, run Python server
ESP32 sends data and receives commands
Version 2 dashboard:
http://192.168.4.1/
