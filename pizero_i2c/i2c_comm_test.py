#!/usr/bin/python
import smbus
import time
import sys

# for RPI version 2, we use bus 1"
bus = smbus.SMBus(1)

# This is the address we setup in the Arduino Program
address = 0x08


def writeWire(value):
    byte_list = []
    count = len(value)
    if (count > 1):
        for index in range(1,(count)):
            byte_list.append(ord(value[index]))
    print count,ord(value[0]),byte_list
    bus.write_i2c_block_data(address, ord(value[0]), byte_list)
    return -1

def readWire(numbytes):
    data = ""
    byte_list = bus.read_i2c_block_data(address, 0, numbytes)
    print byte_list
    for byte in byte_list:
        if (byte == 0):
            return data
        data = data + chr(byte)
    return data

print "Type the string to send between \"\""
print "Like: \"data to send\""
while True:
    var = input("")
    if not var:
        continue
    print "Sending '%s'" % (var)
    writeWire(var)
    time.sleep(1)
    print "Receiving:"
    print readWire(20)

