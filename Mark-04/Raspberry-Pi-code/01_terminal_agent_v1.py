#!/usr/bin/python3
import smbus
import time
import sys
import signal
import os
import socket
from gpiozero import Button
from subprocess import check_call

# for RPI version 2, we use bus No. 1
bus = smbus.SMBus(1)

# This is the target address we setup in the Arduino Program
address = 0x08


# send string to the i2c wire
def writeWireString(value):
    byte_list = []
    count = len(value)
    if (count > 1):
        for index in range(1,(count)):
            byte_list.append(ord(value[index]))
    print (count,ord(value[0]),byte_list)
    try:
        bus.write_i2c_block_data(address, ord(value[0]), byte_list)
    except IOError:
        print ("Error: IOError occured while sending")
        return 1
    return 0


# read number of bytes from the i2c wire
# and return it as a string
def readWireString(numbytes):
    byte_list = []
    data = ""
    try:
        byte_list = bus.read_i2c_block_data(address, 0, numbytes)
    except IOError:
        print ("Error: IOError occured while receiving")
        return data
    print (byte_list)
    for byte in byte_list:
        if (byte == 0):
            return data
        data = data + chr(byte)
    return data


# read number of bytes from the i2c wire
# and return them as byte list
def readWire(numbytes):
    byte_list = []
    try:
        byte_list = bus.read_i2c_block_data(address, 0, numbytes)
    except IOError:
        print ("Error: IOError occured while receiving")
    return byte_list


# CTRL+C signal handler 
def signal_handler(signal, frame):
    print ('...Exiting...')
    sys.exit(0)


def send_machine_info():
    hostname = socket.gethostname()
    ipaddress = os.popen("ifconfig wlan0 \
                     | grep 'inet ' \
                     | awk '{print $2}'").read()
    ssid = os.popen("iwconfig wlan0 \
                | grep 'ESSID' \
                | awk '{print $4}' \
                | awk -F\\\" '{print $2}'").read()
    writeWireString("host: " + hostname)
    time.sleep(1)
    writeWireString("ssid: " + ssid.rstrip("\n\r"))
    time.sleep(1)
    writeWireString(ipaddress.rstrip("\n\r"))
    time.sleep(1)

def shutdown_system():
    writeWireString("Shutdown...")
    check_call(['sudo', 'poweroff'])


################################
# MAIN part
################################
if __name__ == '__main__':
    signal.signal(signal.SIGINT, signal_handler)

    shutdown_btn = Button(21, hold_time=2)
    shutdown_btn.when_held = shutdown_system

    going = True
    while going:
        send_machine_info()
        time.sleep(10)

    quit()
