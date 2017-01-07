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
    bus.write_i2c_block_data(address, ord(value[0]), byte_list)
    return -1


# read number of bytes from the i2c wire
# and return it as a string
def readWireString(numbytes):
    data = ""
    byte_list = bus.read_i2c_block_data(address, 0, numbytes)
    print byte_list
    for byte in byte_list:
        if (byte == 0):
            return data
        data = data + chr(byte)
    return data


# read number of bytes from the i2c wire
# and return them as byte list
def readWire(numbytes):
    data = ""
    byte_list = bus.read_i2c_block_data(address, 0, numbytes)
    return byte_list


def signal_handler(signal, frame):
    print '...Exiting...'
    sys.exit(0)

signal.signal(signal.SIGINT, signal_handler)

while True:
    var = input("")
    if not var:
        continue
    print "Sending '%s'" % (var)
    writeWireString(var)
    time.sleep(0.5)
    print "Receiving:"
    print readWire(4)

