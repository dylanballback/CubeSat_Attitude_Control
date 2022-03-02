# CubeSat Attitude Raspberry Pi Control Software 

## Goals
The Raspberry Pi software will recive the IMU and photosensor data, and then be able to send commands to the motor function on the Arduino. This will allow for the CubeSat to be wirelessly controled through python code that can be uploaded through a website.

## To-Do
| Declared | To-Do | Completed |
--- | --- | ---
| 2/8/22 | Define what communication protocal is going to be used between Raspberry Pi and Arduino |
| 2/8/22 | Develop and write the Raspberry Pi function that can send motor comands to Arduino |
| 2/8/22 | Develop and write the Raspberry Pi function that can recive IMU data from the Arduino |
| 2/8/22 | Develop and write the Raspberry Pi function that can recive photo sensor data from the Arduino |
| 2/8/22 | Get Raspberry Pi connected to webserver to upload code from Easycontrols.com |
| 2/8/22 | Develop and write documentation on Raspberry Pi python code for all the control commands to Arduino |


### 2/26/22
- Developed the arduino and python code that would enable for serial communication between the two devices. While experimenting with this, I realized that my original idea of sending the sensor data through the Arduino to the Raspberry Pi via a serial connection and then processing the control code on the Raspberry Pi and sending the motor commands back to the Arduino would be way to slow for this application. [Serial communication between Arduino and Pi Code]()

---

### 3/2/22
- Set up a Raspberry Pi to enable wireless upload of Arduino sketch through the terminal. This is to allow a new arduino sketch to be uploaded wirelessly using a SSH connection to the Raspberry Pi terminal.
