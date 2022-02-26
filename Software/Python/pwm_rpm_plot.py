#!/usr/bin/env python3
import serial
import time

if __name__ == '__main__':
    ser = serial.Serial('/dev/tty.usbserial-1120', 9600, timeout=1)
    ser.reset_input_buffer()
    
    #Basic reading serial data from arduino and printing it
    while True:
        if ser.in_waiting > 0:
            line = ser.readline().decode('utf-8').rstrip()
            pwm = []
            rpm = []
            slice_line = slice(line)
            print(line)
            print(slice_line)
