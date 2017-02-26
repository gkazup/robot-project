#!/usr/bin/python
import smbus
import time
import sys
import signal
import pygame
import cv2
import numpy
from picamera.array import PiRGBArray
from picamera import PiCamera


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

# CTRL+C signal handler 
def signal_handler(signal, frame):
    print '...Exiting...'
    sys.exit(0)

# handle keypresses
def handle_keypress():
    going = True
    change = False
    for e in pygame.event.get():
        if (e.type == pygame.QUIT):
            going = False
        if (e.type == pygame.KEYDOWN):
            if (e.key == pygame.K_ESCAPE):
                going = False
            elif (e.key == pygame.K_LEFT):
                left = True
                change = True
            elif (e.key == pygame.K_RIGHT):
                right = True
                change = True
            elif (e.key == pygame.K_UP):
                up = True
                change = True
            elif (e.key == pygame.K_DOWN):
                down = True
                change = True
        if e.type == pygame.KEYUP:
            if (e.key == pygame.K_LEFT):
                left = False
                change = True
            elif (e.key == pygame.K_RIGHT):
                right = False
                change = True
            elif (e.key == pygame.K_UP):
                up = False
                change = True
            elif (e.key == pygame.K_DOWN):
                down = False
                change = True

    if (change):
        if (up and left):
            writeWireString("tf 50 150")
        elif (up and right):
            writeWireString("tf 150 50")
        elif (down and left):
            writeWireString("tb 50 150")
        elif (down and right):
            writeWireString("tb 150 50")
        elif (up):
            writeWireString("f 150")
        elif (down):
            writeWireString("b 150")
        elif (left):
            writeWireString("tl 150 150")
        elif (right):
            writeWireString("tr 150 150")
        if ( (not up) and (not down) and (not left) and (not right) ):
            writeWireString("s")
        change = False

    return going

def getCamFrame(camera,rawCapture):
#    camera.capture(rawCapture, format="bgr")
#    frame = rawCapture.array
    retval,frame=camera.read()
    frame = cv2.cvtColor(frame,cv2.COLOR_BGR2RGB)
    #frame=numpy.rot90(frame)
    frame = pygame.surfarray.make_surface(frame)
    return frame

def blitCamFrame(frame,screen):
    screen.blit(frame,(0,0))
    return screen


# MAIN part
signal.signal(signal.SIGINT, signal_handler)

print "Initalizing..."
pygame.init()
pygame.key.set_repeat()
screen_width, screen_height = 640, 480
screen = pygame.display.set_mode((screen_width,screen_height))
pygame.display.set_caption("Video Control")
screen.fill(0)

camera = PiCamera()
camera.resolution = ((screen_width,screen_height))
camera.framerate = 16
#rawCapture = PiRGBArray(camera, size=(screen_width,screen_height))
rawCapture = PiRGBArray(camera)

print "press ESC to exit"
left = False
right = False
up = False
down = False
writeWireString("e")

going = True
while going:
    going = handle_keypress()

    frame = getCamFrame(camera,rawCapture)
    screen = blitCamFrame(frame,screen)
    pygame.display.flip()
    pygame.time.wait(10)

# cleanup
writeWireString("s")
writeWireString("d")
quit()

