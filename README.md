# robot-project
code repository for the robot project
The goal of this project is to create a self-driving tracked robot that is capable to move around and navigate in the house on it's own.
It will be guided by various sensors:
- video camera for OpenCV
- touch sensors
- ultrasonic sensors

## Mark-04
Using Raspberry Pi 3 instead of a the Pi Zero.
- ROS integration
- new remote client built on websockets

## Mark-03
Ultrasonic sensors are ditched for a video camera.
- remote control option has now a video feed integrated.
- board layout streamlined
<img src="/doc/mark-03_small.jpg"/>

## Mark-02
With 3 ultrasonic sensors at the front for sensor coverage.
Control options:
- command line interface
- gamepy with arrow keys for natural control
<img src="/doc/mark-02_small.jpg"/>

## Mark-01
The basic robot chassis
<img src="/doc/mark-01_small.jpg"/>
