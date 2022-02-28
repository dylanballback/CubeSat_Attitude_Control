# Matlab 1 degree of freedom reaction wheel inverted pendulum

## Goals
The goal of this is to develop a mathmatical model that simulated the real world version of the 1 degree of freedom reaction wheel inverted pendulum so that the PID constants can be calculated to reach the desired goal instead of just guessing and checking random values in the PID. 

## To-Do
| Declared | To-Do | Completed |
--- | --- | ---
| 2/20/22 | ~~Derive equations of motion for 1 DoF reaction wheel inverted pendulum system.~~ | 2/23/22
| 2/20/22 | ~~Transfer the equatoins of motion into Matlab script.~~ | 2/23/22
| 2/20/22 | ~~Create simulink model of a PID with the EOM from Matlab script.~~ | 2/23/22
| 2/20/22 | Tune values of PID based on simulink model and implment in real life model. | 2/24/22









## Updates 

### 2/23/22
- Derived equations of motion for the 1 degree of freedom reaction wheel inverted pendulum system and programmed them into a matlab model with Dr. White (Bryan)
- Created a simulink PID model based off the Matlab, tuned PID based on the model with Dr. White

---

### 2/24/22
- Implemented simulinks tuned PID constants into real life model on arduino. Discovered that the system was not even close to being correctly tuned.

---

### 2/25/22 
- Talked with Dr. White about the implementation of the Matlab/Simulink model to the real world model on the arduino.
    - Discovered that arduino had angle units based off degrees/sec while Matlab/Simulink was in Rad/s.
    - Discovered that Matlab/Simulink model PID output was in Amp while Arduino code was controling PWM (Voltage).
    - Discovered that Matlab/Simulink model PID had no clamp on output Amp where the motor can handle a maxium of 1.1 A according to its data sheet.

- Worked with Dr. White to derive equation to convert the output of the Matlab/Simulink model from Amp to Voltage to PWM signal so that the simulation made from matlab can actually be implemented on our arduino and physical system.
