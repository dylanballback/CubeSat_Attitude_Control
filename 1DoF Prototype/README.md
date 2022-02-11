# 1 Degree of Freedom Reaction Wheel Control Prototype

## Goals
The goal of this is to first protoype how to balance a 1 degree of freedom system before jumping stright into a 6 degree of freedom system. This initial test will teach us how to operate a single motor and read IMU data. Additionally learning how to make a simple PID controller to balance a 1 Dof system. Allow us to measure how much energy is required with one motor in a single degree of freedom. 

## To-Do
- (2/8/22): Design and 3D print 1DoF test stand. Completed (2/8/22)
- (2/8/22): Design and 3D print reaction wheel. Completed (2/8/22)
- (2/8/22): Mount motor and IMU to test stand. Completed (2/8/22)
- (2/8/22): Connect Motor and IMU to Arduino to test basic functions. Completed (2/9/22)
- (2/8/22): Attempt to get PID Code to balance the reaction wheel. 


### Updates 

- 2/8/22
    - Designed, 3D printed, and assembled one degree of freedom reaction wheel testbed.
    <p float="left">
        <img src="https://github.com/dylanballback/CubeSat_Attitude_Control/blob/main/Images/CAD_1DOF_testbed_V1.jpeg" width="350" >
        <img src="https://github.com/dylanballback/CubeSat_Attitude_Control/blob/main/Images/front_1DoF_testbed_V1.jpeg" width="350" >
        <img src="https://github.com/dylanballback/CubeSat_Attitude_Control/blob/main/Images/side_1DoF_testbed_V1.jpeg" width="350" >
        <img src="https://github.com/dylanballback/CubeSat_Attitude_Control/blob/main/Images/back_1DoF_testbed_V1.jpeg" width="350" >
    </p>
    

- 2/9/22
    - Wired one Nidec 24H404H BLDC Motor to Arduino and wrote a program for it to spin in both directions.
    - Got MPU6050 with Kalman Filter up and runing reliably with the Arduino.


- 2/10/22
    - Testing and data collection of RPM with relation to voltage and PWM signal using Encoder
    - Attempted and wrote two different versions of a PID controller
    - Found out I need to research more about PID tunning methods, and size and weight of the reaction wheel
    - Designed and printed version 2 of reaction wheel with increased radius and weight
    - Trying https://en.wikipedia.org/wiki/Ziegler%E2%80%93Nichols_method for PID tunning