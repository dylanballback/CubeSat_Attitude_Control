# CubeSat Attitude Raspberry Pi Control Software 

## Goals
The goal is to allow for the CubeSat to be wirelessly controlled through code that can be uploaded through a website.
This is section is where I am testing the best method to wirelessly upload code.

## To-Do
| Declared | To-Do | Completed |
--- | --- | ---
| 2/8/22 | ~Define what communication protocal is going to be used between Raspberry Pi and Arduino~ | 2/26/22
| 2/8/22 | ~Develop and write the Raspberry Pi function that can send commands to Arduino~| 2/26/22
| 2/8/22 | ~Develop and write the Raspberry Pi function that can recive data from the Arduino~ | 2/26/22
| 2/8/22 | ~Set up Raspberry Pi so it can upload an Arduino sketch through the terminal~ | 3/2/22
| 3/2/22 | Set up git for remote upload through Raspberry Pi SSH arduino code upload |
| 3/2/22 | Develop bash script to automatically update code from Raspberry Pi to Arduino |
| 2/8/22 | Get Raspberry Pi connected to webserver to upload code from Easycontrols.com |
| 2/8/22 | Develop and write documentation on Raspberry Pi python code for all the control commands to Arduino |


## Updates 

### 2/26/22
- Developed the arduino and python code that would enable for serial communication between the two devices. While experimenting with this, I realized that my original idea of sending the sensor data through the Arduino to the Raspberry Pi via a serial connection and then processing the control code on the Raspberry Pi and sending the motor commands back to the Arduino would be way to slow for this application. [Serial communication between Arduino and Pi Code](https://github.com/dylanballback/CubeSat_Attitude_Control/tree/main/Software/Raspberry%20Pi%20%26%20Arduino%20Com/Serial%20Communication)

---

### 3/2/22
- Set up a Raspberry Pi to enable wireless upload of Arduino sketch through the terminal. This is to allow a new arduino sketch to be uploaded wirelessly using a SSH connection to the Raspberry Pi terminal. This approach to wirelessly upload code to the arduino solves the previous issue of using serial communication to send and recieve data between the two devices since there is no time delay of communcication while the main program is being ran. 

