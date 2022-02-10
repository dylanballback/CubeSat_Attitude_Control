# 1 Degree of Freedom Reaction Wheel Control Prototype

## Goals
The goal of this is to first protoype how to balance a 1 degree of freedom system before jumping stright into a 6 degree of freedom system. This initial test will teach us how to operate a single motor and read IMU data. Additionally learning how to make a simple PID controller to balance a 1 Dof system. Allow us to measure how much energy is required with one motor in a single degree of freedom. 

## To-Do
- (2/8/21): Design and 3D print 1DoF test stand. Completed (2/8/21)
- (2/8/21): Design and 3D print reaction wheel. Completed (2/8/21)
- (2/8/21): Mount motor and IMU to test stand. Completed (2/8/21)
- (2/8/21): Connect Motor and IMU to Arduino to test basic functions. Completed (2/9/21)
- (2/8/21): Attempt to get PID Code to balance the reaction wheel. 


### Updates 

- 2/8/21
    - Designed, 3D printed, and assembled one degree of freedom reaction wheel testbed.
    <img src="https://github.com/dylanballback/CubeSat_Attitude_Control/blob/main/Images/front_1DoF_testbed_V1.JPG" width="700" >
    <img src="https://github.com/dylanballback/CubeSat_Attitude_Control/blob/main/Images/side_1DoF_testbed_V1.JPG" width="700" >
    <img src="https://github.com/dylanballback/CubeSat_Attitude_Control/blob/main/Images/back_1DoF_testbed_V1.JPG" width="700" >

- 2/9/21
    - Wired one Nidec 24H404H BLDC Motor to Arduino and wrote a program for it to spin in both directions.
    - Got MPU6050 with Kalman Filter up and runing reliably with the Arduino.
