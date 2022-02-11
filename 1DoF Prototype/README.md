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
| 2/11/22 | Research equations of motions for 1DoF. Calculate reaction wheel required moment of inertia. |
| 2/11/22 | Research and attempt differnet methods for tuning PID controller. |
| 2/11/22 | Achive 1DoF Reaction Wheel Balanced with tuned PID controller. |




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
