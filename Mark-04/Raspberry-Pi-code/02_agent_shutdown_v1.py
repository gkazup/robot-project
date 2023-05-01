#!/usr/bin/python3
from gpiozero import Button
from subprocess import check_call
from signal import pause
import sys

offGPIO = int(sys.argv[1]) if len(sys.argv) >= 2 else 21
holdTime = int(sys.argv[2]) if len(sys.argv) >= 3 else 5


def shutdown_system():
    check_call(['sudo', 'poweroff'])


################################
# MAIN part
################################
if __name__ == '__main__':
    shutdown_btn = Button(offGPIO, hold_time=holdTime)
    shutdown_btn.when_held = shutdown_system

    pause()
