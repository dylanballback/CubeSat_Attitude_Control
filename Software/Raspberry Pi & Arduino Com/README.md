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

<p float="left">
    <img src="https://github.com/dylanballback/CubeSat_Attitude_Control/blob/main/Images/Software/pi_send_serial_data_test.png" width="500" >
    <img src="https://github.com/dylanballback/CubeSat_Attitude_Control/blob/main/Images/Software/serial_test_from_ardruino.png" width="500" >
</p>
    

---

### 3/2/22
- Set up a Raspberry Pi to enable wireless upload of Arduino sketch through the terminal. This is to allow a new arduino sketch to be uploaded wirelessly using a SSH connection to the Raspberry Pi terminal. This approach to wirelessly upload code to the arduino solves the previous issue of using serial communication to send and recieve data between the two devices since there is no time delay of communcication while the main program is being ran. [SSH into Pi and upload new sketch through terrminal with PlatformIO](https://github.com/dylanballback/CubeSat_Attitude_Control/tree/main/Software/Raspberry%20Pi%20%26%20Arduino%20Com/SSH%20remote%20upload)

---

### 3/5/22
- Set up the Raspberry Pi Zero with github to enable for me to update the arduino sketch on my laptop and then push it to github. Then go through the Raspberry Pi SSH connection and git pull the updated code and upload it to the Arduino throught the Pi with Platformio. This got rid of the need to use Nano or Vim on the Pi terminal window for programing which was not very user friendly and lacks features that VS code has. [Github link for easily uploading code to Raspberry Pi Zero]https://github.com/dylanballback/Arduinoproject)

---

### 3/6/22
- Spent the day attemping to get the serial communication between the Raspberry Pi Zero working through the GPIO Rx and Tx pins instead of just a USB cable. I had to use a voltage shifter since the Raspberry Pi Zero runs at 3.3V logic and the Arduino Nano at 5V logic. I was unsecusesful in my attempts to get the serial communication working between the two devices using the Rx and Tx GPIO pin. I will have to revisit this and get it working to reduce the size of the PCB. 

---

### 3/7/22
- Working on the Serial bidirectional communication between the Raspberry Pi Zero and Arduino Nano. I was attempting to see if I could increase the Baud rate of the serial communication to decrease the communication delay between the two devices. I was able to increase the Baud rate and am able to easily recive data from one device but run into a lot of issues when I try to recive and send data from both devices. I am able to achive bidirectional serial communcication between the devices but I get a big delay of data, I think it has something to do with trying to send and recive data at the same time. I think the code should be set up to send and recive the data at certain times. Also another thing I was thinking about was the speed in which the Arduino Nano is able to operate and run code at versus the Raspberry Pi Zero python program run time difference in speed could additional be causing issues. 

- I think moving forward I am trying to aquire either an ESP32 or Arduino Nano 33 IOT where the microcontroller itself is able to connect to the internet. Therefore I would be able to wireless upload code easily through the internet to the microcontroller and not have to worry about different programs that interfer with that like I had issues with the PlatformIO custom librarys. 
- Another approach would be to get the Raspberry PI Zero to read the IMU data and the only send motor commands to the Arduino Nano. Then I have to think about the delay in which the Pi IMU data collection and more serial communication issues. 

---