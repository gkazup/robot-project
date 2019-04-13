# robot-project
Code repository for the robot project.
The goal of this project is to create a self-driving tracked robot that is capable to move around and navigate in the house on it's own.

It will be guided by various sensors:
- video camera for OpenCV
- touch sensors
- ultrasonic sensors

## Mark-04
Full redesign
- using Raspberry Pi 3 instead of a Pi Zero
- using a robodyne compact Arduino Mega 2560
- now with a robot arm

## Mark-03
Ultrasonic sensors are ditched for a video camera.
- remote control option has now a video feed integrated.
- board layout streamlined
<img src="/doc/mark-03_small.jpg"/>

## Mark-02
With 3 ultrasonic sensors at the front for sensor coverage.

The motor driver and ultrasonic sensors are controlled by the Arduino. The raspberry Pi Zero is handling the high level business logic. Communication between the Arduino and Pi is through I2C.

Control options:
- command line interface
- gamepy with arrow keys for natural control
<img src="/doc/mark-02_small.jpg"/>

## Mark-01
The basic robot chassis.

Components list:
- T200 Silver Aluminum tank car chassis
- 8.4V 26650 Bike Light Battery Pack (36000mAh 6x26650 Rechargeable Li-ion Battery Pack For Bicycle Light)
- 5.5*2.1mm DC Male Cable 24C/0.20 22Awg 12V
- AMS1117 4.5-7V Turn 3.3V DC-DC Step Down Power Supply Module 
- 22AWG 2pin power cord copper
- Dual-Motor-Driver 1A TB6612FNG
- FT232RL FTDI USB 3.3V 5.5V to TTL Serial Adapter Module
- Pro Mini 328 Mini ATMEGA328 3.3V 8MHz
- MP1584 Ultra-small size adjustable DC-DC step-down power supply module 3A
- 1000 uF 50V capacitor
- USB dual socket
- 802.11n USB WiFi adapter
- Raspberry Pi Zero
- Micro USB plugs
<img src="/doc/mark-01_small.jpg"/>
