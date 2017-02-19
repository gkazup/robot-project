#!/usr/bin/python
import smbus
import time
import sys
import signal

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
    print count,ord(value[0]),byte_list
    try:
        bus.write_i2c_block_data(address, ord(value[0]), byte_list)
    except IOError:
        print "Error: IOError occured while sending"
    return -1


# read number of bytes from the i2c wire
# and return it as a string
def readWireString(numbytes):
    byte_list = []
    data = ""
    try:
        byte_list = bus.read_i2c_block_data(address, 0, numbytes)
    except IOError:
        print "Error: IOError occured while receiving"
    print byte_list
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
        print "Error: IOError occured while receiving"
    return byte_list


def signal_handler(signal, frame):
    print '...Exiting...'
    sys.exit(0)

signal.signal(signal.SIGINT, signal_handler)


var = raw_input("")
print "Input '%s'" %(var)
data = readWire(15)
print data
writeWireString("e")
time.sleep(0.1)
writeWireString("f 150")
time.sleep(0.1)

going = True
while going:
    data = readWire(15)
    print data
    if data[12] < 20:
        writeWireString("s")
        going = False
    time.sleep(0.1)
