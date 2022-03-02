#!/usr/bin/env python3
import serial
import time
import numpy as np
import matplotlib.pyplot as plt


trial = 0
pwm_storage = []
rpm_storage = []
pwm = []
rpm = []

kw = []
kw_avg_storage = []



#Maps a value from one range of numbers to another range of numbers
def map(x, in_min, in_max, out_min, out_max):
    return float((x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min)


#Find the average value of a list
def average(lst):
    return sum(lst)/len(lst)

#Ex.  print("Average of the list =", round(average, 2))


def calculate_kw(pwm, rpm):
    Vmot = map(pwm, 0, 380, 0, 24)
    kw = Vmot / rpm
    return kw



if __name__ == '__main__':
    ser = serial.Serial('/dev/tty.usbserial-1120', 9600, timeout=1)
    ser.reset_input_buffer()
    
    #Basic reading serial data from arduino and printing it
    while True:
        if (len(pwm)) < 380:
            if ser.in_waiting > 0:
                data = ser.readline().decode('utf-8').rstrip()
                slice_data = data.split(",")
                pwm.append(slice_data[0])
                rpm.append(slice_data[1])
                #print(data)
                #print(slice_data)
                #print(pwm)
                #print(rpm)
        elif (len(pwm) == 380):
            zip_pwm_rpm = zip(pwm,rpm) 
            for element1, element2 in zip_pwm_rpm: 
                pwm_value = float(element1)
                rpm_value = float(element2)
                if ((rpm_value > 0) and (pwm_value > 0)):
                    kw_calc = calculate_kw(pwm_value, rpm_value)
                    kw.append(kw_calc)
            kw_avg = average(kw)
            print(kw_avg)
            kw_avg_storage.append(kw_avg)

            #Add collected data from PWM and RPM into a list to save past trials
            pwm_storage.append(pwm)
            rpm_storage.append(rpm)
            #Reset pmw, rpm, and kw_storage lists 
            pwm = []
            rpm = []
            kw = []
            #Add one to indicate the next trial
            trial = trial + 1 
            print(trial)
        elif (trial == 4):
            overall_kw_avg = average(kw_avg_storage)
            print(overall_kw_avg)
            
                
            
""""


def save_data(newfilename, trial, data):
    #Create new files to store data
    new_file = open(newfilename+trial+".txt", "x")
    for element in new_file:
        new_file.write(element + "\n")
    new_file.close()

#Calculates slope of line and constant
def slope(x1, x2, y1, y2):
    m = (y2-y1)/(x2-x1)
    c = y1 - m*x1
    return m, c



#Plot pwm over rpm
def graph(p,r):
    plt.plot(p,r)
    plt.title("Motor Velocity over PWM Signal (@24V)")
    plt.xlabel("PWM Signal")
    plt.ylabel("Motor RPM")
    plt.show() 
"""