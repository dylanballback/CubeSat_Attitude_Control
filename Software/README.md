# CubeSat Attitude Control Software 

## Goals
The goal is to develop a PID control algorithum that will allow for the 1U CubeSat be orentied as the user defines. There are two main software functionalities that need to be developed. The first being for the Arduino which will have functions to read the IMU data, photosensor, and control the direction and speed of the motors in the CubeSat. The Raspberry Pi will enable for new code to be wirelessly uploaded to the Arduino. This will allow for the CubeSat to be wirelessly controled through a file that is uploaded to a website.

## To-Do
| Declared | To-Do | Completed |
--- | --- | ---
| 2/8/22 | ~Develop and write functions for Arduino to read IMU data and control motor~ | 2/26/22
| 2/8/22 | ~Develop and write the Arduino function that can send/recive data to the Raspberry Pi~ | 2/26/22
| 2/8/22 | ~Develop and write the Raspberry Pi function that can send/recive data from the Arduino~ | 2/26/22
| 2/8/22 | ~Develop function that can wirelessly upload Arduino code through the Raspberry Pi~ | 3/2/22
| 2/8/22 | Get Raspberry Pi connected to webserver to upload code from Easycontrols.org |
| 2/8/22 | Develop and write documentation on Raspberry Pi control commands to Arduino |

## Updates 

### 2/26/22
- Developed serial communication between Arduino and Raspberry Pi. [See more information about this here](https://github.com/dylanballback/CubeSat_Attitude_Control/tree/main/Software/Raspberry%20Pi%20%26%20Arduino%20Com)


---

### 3/2/22

- Working on different methods of Raspberry Pi and Arudino communication can see more updates [See more information about this here](https://github.com/dylanballback/CubeSat_Attitude_Control/tree/main/Software/Raspberry%20Pi%20%26%20Arduino%20Com)


---

### 3/21/22

- Experimenting with websockets and WiFi enabled Arduino to be able to wirelessly recive and transmit data between the Arduino. 

---

