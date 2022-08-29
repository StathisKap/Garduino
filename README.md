# Garduino

A project inspired by the YouTube Channel **Practical Engineering** on [this video](https://www.youtube.com/watch?v=O_Q1WKCtWiA)

An Arduino Uno Wifi Rev 2 collect temperature and humidity data every 15 minutes from the soil using 2 analog sensors, and from the air using 1 digital sensor.
It then uses the WIFININA library to send the data from the sensors to a timeseries database called [InfluxDB](https://github.com/influxdata/influxdb) on a [Home Assistant](https://github.com/home-assistant/core) VirtualBox Virtual Machine
using the inbuilt REST API.

The arduino also controls a 12V 3/4 inch valve, that opens once a day for 15 minutes, provided it has enough pressure from the hose at the input end.

## Use
To connect the Arduino to Wifi, **you'll need to create a file named arduino_secrets.h** and define a few macros in there like so:

```text
───────┬───────────────────────────────────────────
       │ File: arduino_secrets.h
───────┼───────────────────────────────────────────
   1   │ #define SECRET_SSID     "WIFI_NAME"
   2   │ #define SECRET_PASS     "WIFI_PASSWORD"
   3   │ #define DB_USER         "USERNAME THAT THE ARDUINO WILL USE TO LOG INTO THE DATABASE"
   4   │ #define DB_PASS         "PASSWORD THAT THE ARDUINO WILL USE TO LOG INTO THE DATABASE"
───────┴───────────────────────────────────────────
```

To do the wiring for the Solenoid Valve, follow this tutorial: https://bc-robotics.com/tutorials/controlling-a-solenoid-valve-with-arduino/  
To do the wiring for the DHT22 temperature and humidity sensor for the air, use this guide: https://create.arduino.cc/projecthub/mafzal/temperature-monitoring-with-dht22-arduino-15b013  
To do the wiring for the LM35 soil temperature sensor, use this guide: https://lastminuteengineers.com/lm35-temperature-sensor-arduino-tutorial/   
To do the wiring for the soil humidity sensor, use this guide: https://create.arduino.cc/projecthub/MisterBotBreak/how-to-use-a-soil-moisture-sensor-ce769b  

## Parts List
- 12V SOLENOID VALVE - 3/4"  -  €7.9800 
- ARDUINO UNO WIFI REV2 - €40.2400
- BATTERY HOLDER 1X9V WITH JST CONNECTOR & SWITCH - €0.7300
- TEMPERATURE SENSOR - WATERPROOF (LM35) - €4.4400
- WAVESHARE TEMPERATURE-HUMIDITY SENSOR DHT22 - €8.7900
- 12V Lead Battery - €6.90
- TIP120 Darlington Transistor - €0.50
- 1K Ohm Resistor - €0.0200
- 1N4001 Diode - €0.04

**Total cost** (without VAT, wires, connectors, solder, and the tools):  
> €70.14