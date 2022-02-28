# 1 Degree of Freedom Reaction Wheel Control Prototype

## Goals
The goal of this is to first protoype how to balance a 1 degree of freedom system before jumping stright into a 6 degree of freedom system. This initial test will teach us how to operate a single motor and read IMU data. Additionally learning how to make a simple PID controller to balance a 1 Dof system. Allow us to measure how much energy is required with one motor in a single degree of freedom. 

## To-Do
| Declared | To-Do | Completed |
--- | --- | ---
| 2/8/22 | ~~Design and 3D print 1DoF test stand.~~ | 2/8/22
| 2/8/22 | ~~Design and 3D print reaction wheel.~~ | 2/8/22
| 2/8/22 | ~~Mount motor and IMU to test stand.~~ | 2/8/22
| 2/8/22 | ~~Connect Motor and IMU to Arduino to test basic functions.~~ | 2/9/22
| 2/8/22 | ~~Attempt to get PID Code to balance the reaction wheel.~~ | 2/10/22
| 2/11/22 | ~~Get working untuned PID Code for the reaction wheel.~~ | 2/11/22
| 2/11/22 | ~~Research equations of motions for 1DoF. Calculate reaction wheel required moment of inertia.~~ | 2/15/22
| 2/11/22 | Research and attempt differnet methods for tuning PID controller. |
| 2/11/22 | Achive 1DoF Reaction Wheel Balanced with tuned PID controller. |
| 2/16/22 | Write function to read IMU angle through use of Z axis for more accurate readings. |
| 2/16/22 | Write function for PID to output reaction wheel acceleration. |
| 2/16/22 | Write function for motor to reach desired acceleration recieved from PID function. |






## Updates 

### 2/8/22
- Designed, 3D printed, and assembled one degree of freedom reaction wheel testbed.
<p float="left">
    <img src="https://github.com/dylanballback/CubeSat_Attitude_Control/blob/main/Images/1Dof_testbed/CAD_1DOF_testbed_V1.jpeg" width="400" >
    <img src="https://github.com/dylanballback/CubeSat_Attitude_Control/blob/main/Images/1Dof_testbed/front_1DoF_testbed_V1.jpeg" width="500" >
    <img src="https://github.com/dylanballback/CubeSat_Attitude_Control/blob/main/Images/1Dof_testbed/side_1DoF_testbed_V1.jpeg" width="400" >
    <img src="https://github.com/dylanballback/CubeSat_Attitude_Control/blob/main/Images/1Dof_testbed/back_1DoF_testbed_V1.jpeg" width="500" >
</p>
    
---


### 2/9/22
- Wired one Nidec 24H404H BLDC Motor to Arduino and wrote a program for it to spin in both directions. ([Basic Motor Test Code](https://github.com/dylanballback/CubeSat_Attitude_Control/blob/main/1DoF%20Prototype/Arduino/motor_test_basic/motor_test_basic.ino))
<p style="text-align:center; float="left">
    <img src="https://github.com/dylanballback/CubeSat_Attitude_Control/blob/main/Images/1Dof_testbed/motor_run_breadboard1.jpeg" width="250" style="float: left">
    <img src="https://github.com/dylanballback/CubeSat_Attitude_Control/blob/main/Images/1Dof_testbed/pwm_duty_cycle_test1.jpeg" width="375" style="float: right" >
    <img src="https://github.com/dylanballback/CubeSat_Attitude_Control/blob/main/Images/1Dof_testbed/pwm_duty_cycle_test2.jpeg" width="375" style="float: right">
    Testing relationship between PWM duty cycle and motor speed with Osciliscope along with maximum PWM signal that can be sent.
</p>

- Got MPU6050 with Kalman Filter up and runing reliably with the Arduino. ([MPU6050 Kalmain Filter Code](https://github.com/dylanballback/CubeSat_Attitude_Control/blob/main/1DoF%20Prototype/Arduino/MPU6050_Kalman_Filter/MPU6050_Kalman_Filter.ino))

---


### 2/10/22
- Testing and data collection of RPM with relation to voltage and PWM signal using Encoder. ([Encoder RPM Test Code](https://github.com/dylanballback/CubeSat_Attitude_Control/blob/main/1DoF%20Prototype/Arduino/encoder_test/encoder_test.ino))
    <img src="https://github.com/dylanballback/CubeSat_Attitude_Control/blob/main/Images/1Dof_testbed/encoder_1Dof_test1.jpeg" width="400" >

- Attempted and wrote two non-working different versions of a PID controller. ( [Balance PID Code V1](https://github.com/dylanballback/CubeSat_Attitude_Control/blob/main/1DoF%20Prototype/Arduino/balance_V1/balance_V1.ino) )( [Balance PID Code V2](https://github.com/dylanballback/CubeSat_Attitude_Control/blob/main/1DoF%20Prototype/Arduino/balance_V2/balance_V2.ino))

- Found out I need to research more about PID tunning methods, and size and weight of the reaction wheel.
<p float="left">
    <img src="https://github.com/dylanballback/CubeSat_Attitude_Control/blob/main/Images/1Dof_testbed/CAD_1Dof_testbed_V2.jpeg" width="400" >
    <img src="https://github.com/dylanballback/CubeSat_Attitude_Control/blob/main/Images/1Dof_testbed/CAD2_1Dof_testbed_V2.jpeg" width="400" >
</p>

- Designed and printed version 2 of reaction wheel with increased radius and weight.
- Trying [Ziegler–Nichols method](https://en.wikipedia.org/wiki/Ziegler%E2%80%93Nichols_method) for PID tunning.


---

### 2/11/22
- Achived a working PID program that maps out PWM values within operating range of the motor. ([Untuned Balance PID code V3](https://github.com/dylanballback/CubeSat_Attitude_Control/blob/main/1DoF%20Prototype/Arduino/balance_V3/balance_V3.ino))


---


### 2/12/22
- Continued to work on Tuning PID code. There are issues with mapping pwm values.


---


### 2/13/22
- Worked on ploting PID output data and current angle over time in serial plotter to help with tuning PID. ([Untuned Balance PID code V4](https://github.com/dylanballback/CubeSat_Attitude_Control/blob/main/1DoF%20Prototype/Arduino/balance_V4/balance_V4.ino))


---

### 2/14/22 

- Working on different PID Arduino code, trying variety of differnt methods comparing if the results of my version of the PID code matches the output of a PID code using the Arduino PID package, Version 5, and Version 6.  ([Untuned Balance PID code V5](https://github.com/dylanballback/CubeSat_Attitude_Control/blob/main/1DoF%20Prototype/Arduino/balance_V5/balance_V5.ino))  ([Untuned Balance PID code V6](https://github.com/dylanballback/CubeSat_Attitude_Control/blob/main/1DoF%20Prototype/Arduino/balance_V6/balance_V6.ino))

---

### 2/15/22

- Working on derivating the equations of motion for a 1 degree of freedom system, and attemping to create a matlab simualtion to help with tunning the PID. ([EOM for 1DoF](https://github.com/dylanballback/CubeSat_Attitude_Control/blob/main/1DoF%20Prototype/Notes/1DOF%20EOM%20Feb%2015%2C%202022.pdf))
- Increased the outter diameter of the reaction wheel version 1. I discovered that the most efficent design of the reaction wheel is to achive the largest moment of inertia with the lowest mass. This can be achived through increasing the diameter of the wheel.
-Modified CAD of 1DoF base to allow for forces to be applied on bearing ring without preventing the rotation of the bearing. Additional, added a small stop for both side of the base.
<p style="text-align:center; float="left">
    <img src="https://github.com/dylanballback/CubeSat_Attitude_Control/blob/main/Images/1Dof_testbed/reaction_wheel_V3.png" width="300" style="float: left">
    <img src="https://github.com/dylanballback/CubeSat_Attitude_Control/blob/main/Images/1Dof_testbed/1DOF_base_front.png" width="400" style="float: left" >
</p>
<p style="text-align:center; float="right">   
    <img src="https://github.com/dylanballback/CubeSat_Attitude_Control/blob/main/Images/1Dof_testbed/1DOF_base_side.png" width="300" style="float: right">
    <img src="https://github.com/dylanballback/CubeSat_Attitude_Control/blob/main/Images/1Dof_testbed/1DOF_base_back.png" width="400" style="float: right">
</p>    

---

### 2/16/22

- I discovered that limits have to be set for the integral term in the PID along with the overall PID output. Before I was just constraining the overall PID output to be within range of my motor, but the I (integral term) was not constrained. ([Youtube Source](https://www.youtube.com/watch?v=NVLXCwc8HzM)) 
- Rewrote PID balance code to include function to set limits of both the I (integral term) and the overall PID output. ([Untuned Balance PID code V7](https://github.com/dylanballback/CubeSat_Attitude_Control/blob/main/1DoF%20Prototype/Arduino/balance_V7/balance_V7.ino))
- Working on collecting individual motor data for both at 12V and 24V and with no load, and the reaction wheel load.
- Rewrote PID balance code so that the change in direction of the motor is based off the PID output itself, previously I had the direction flip based off the angle. ([Untuned Balance PID code V8](https://github.com/dylanballback/CubeSat_Attitude_Control/blob/main/1DoF%20Prototype/Arduino/balance_V8/balance_V7.ino))

---

### 2/17/22

- Worked on new attempt of getting the PID to work, different trials with larger motor step sizes. realized that this was not the smartest way to modify the PID loop to achive our desired goal since I was adding more variables to the eqution. ((([Untuned Balance PID code V9](https://github.com/dylanballback/CubeSat_Attitude_Control/blob/main/1DoF%20Prototype/Arduino/balance_V7/balance_V9.ino))

---

### 2/25/22
- Worked with Dr. White to derive EOM for our system and build a matlab model with a simulink PID. Discovered that our matlab model PID output is in Amps, so we derived an equation to covert Amp to Voltage for the motor, which required me to calulate the back EMF for the motor. ([All Matlab code](https://github.com/dylanballback/CubeSat_Attitude_Control/tree/main/Software/Matlab)) 

- Wrote a python script that automates calulating the motor back emf over muiltiple trials and outputing the average of all the trials. I used python to read the serial data from the arduino and parse and run some calulation functions.
([Python code that calulates motors back emf](https://github.com/dylanballback/CubeSat_Attitude_Control/blob/main/Software/Python/pwm_rpm_plot.py)) ([Arduino code supports the python function that calulates motors back emf](https://github.com/dylanballback/CubeSat_Attitude_Control/blob/main/Software/Python/pwm_over_rpm_graph/pwm_over_rpm_graph.ino)) 



