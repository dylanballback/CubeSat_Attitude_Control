# CubeSat Attitude Control Software 

## Goals
The goal is to develop a PID control algorithum that will allow for the 1U CubeSat be orentied as the user defines. There are two main software functionalities that need to be developed. The first being for the Arduino which will have functions to read the IMU data, photosensor, and control the direction and speed of the motors in the CubeSat. The second being for the Raspberry Pi which will recive the IMU and photosensor data, and then be able to send commands to the motor function on the Arduino. This will allow for the CubeSat to be wirelessly controled through python code that can be uploaded through a website.

## To-Do
| Declared | To-Do | Completed |
| 2/8/22 | Develop and write functions for Arduino to read IMU data and control motor |
| 2/8/22 | Develop and write the Arduino function that can send/recive data to the Raspberry Pi |
| 2/8/22 | Develop and write the Raspberry Pi function that can send/recive data from the Arduino |
| 2/8/22 | Get Raspberry Pi connected to webserver to upload code from Easycontrols.com |
| 2/8/22 | Develop and write documentation on Raspberry Pi control commands to Arduino |