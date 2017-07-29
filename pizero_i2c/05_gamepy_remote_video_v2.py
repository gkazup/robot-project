#!/usr/bin/python
import smbus
import time
import sys
import signal
from threading import Thread
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
def handle_keypress(input):
    going = True
    change = False
    output = 0

    if (1 == (input & 1)):
        left = True
    else:
        left = False
    if (2 == (input & 2)):
        right = True
    else:
        right = False
    if (4 == (input & 4)):
        up = True
    else:
        up = False
    if (8 == (input & 8)):
        down = True
    else:
        down = False

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

    if (left):
        output += 1
    if (right):
        output += 2
    if (up):
        output += 4
    if (down):
        output += 8
    if (going):
        output += 16
    return output

def getCamFrame(camera, rawCapture):
    camera.capture(rawCapture, format="bgr")
    rawframe = rawCapture.array
    #cv2.imshow("Image", rawframe)
    #cv2.waitKey(0) & 0xFF
    frame = cv2.cvtColor(rawframe,cv2.COLOR_BGR2RGB)
    frame = numpy.rot90(frame,3)
    frame = numpy.fliplr(frame)
    return frame

def blitCamFrame(frame, screen):
    frame = pygame.surfarray.make_surface(frame)
    screen.blit(frame,(0,0))
    return screen

# camera thread
# this will update the video in pygame
def threaded_video(screen, wait):
    # init picamera
    camera = PiCamera()
    camera.resolution = ((screen_width,screen_height))
    #camera.framerate = 16
    rawCapture = PiRGBArray(camera)
    # update frames until the main task is going
    while going:
        frame = getCamFrame(camera,rawCapture)
        screen = blitCamFrame(frame, screen)
        pygame.display.flip()
        rawCapture.truncate(0)
        pygame.time.wait(wait)

# MAIN part
signal.signal(signal.SIGINT, signal_handler)

print "Initalizing pygame..."
pygame.init()
pygame.key.set_repeat()
screen_width, screen_height = 320, 208
screen = pygame.display.set_mode((screen_width,screen_height))
pygame.display.set_caption("Video Control")

print "Starting video in new thread..."
if __name__ == "__main__":
    videothread = Thread(target = threaded_video, args = (screen, 300, ))
    videothread.start()

print "Use arrow keys to move, press ESC to exit"
keyinfo = 0
going = True
writeWireString("e")

while going:
    keyinfo = handle_keypress(keyinfo)
    if (16 == (keyinfo & 16)):
        going = True
    else:
        going = False

# cleanup after finished running
print "ESC pressed, cleanup before exiting..."
writeWireString("s")
writeWireString("d")
videothread.join()
print "Exiting"
quit()

