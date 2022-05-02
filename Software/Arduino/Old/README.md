# CubeSat Attitude Arduino Control Software 

## Goals
The Arduino software will have functions to read the IMU data, photosensor, and control the direction and speed of the motors in the CubeSat. It will aditionally need a function that is for communication between the Raspberry Pi to send the IMU data and photosensor data and recive the desired attitude and motor comands.

## To-Do
| Declared | To-Do | Completed |
--- | --- | ---
| 2/8/22 | Define what communication protocal is going to be used between Raspberry Pi and Arduino |
| 2/8/22 | Develop and write the Arduino function that can send IMU data to the Raspberry Pi |
| 2/8/22 | Develop and write the Arduino function that can send Photo sensor data to the Raspberry Pi | 
| 2/8/22 | Develop and write the Arduino function that can recive motor data from the Raspberry Pi |
| 2/8/22 | Develop and write documentation on Arduino code for all the control commands from Raspberry Pi |


## Updates

### 3/8/22 
- Achived a simple working 1 DOF PID controller for the CubeSat mounted in the gimbal locked to only rotate in 1 DOF. []