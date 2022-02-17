/*
 * Author: Automatic Addison
 * Website: https://automaticaddison.com
 * Description: Calculate the angular velocity in radians/second of a DC motor
 * with a built-in encoder (forward = positive; reverse = negative) 
 */
 
// Motor encoder output pulses per 360 degree revolution (measured manually)
#define ENC_COUNT_REV 100
 
// Encoder output to Arduino Interrupt pin. Tracks the pulse count.
#define ENC_IN_RIGHT_A 2
 
// Other encoder output to Arduino to keep track of wheel direction
// Tracks the direction of rotation.
#define ENC_IN_RIGHT_B 4
 
// True = Forward; False = Reverse
boolean Direction_right = true;
 
// Keep track of the number of right wheel pulses
volatile long right_wheel_pulse_count = 0;
 
// One-second interval for measurements
int interval = 1000;
  
// Counters for milliseconds during interval
long previousMillis = 0;
long currentMillis = 0;
 
// Variable for RPM measuerment
float rpm_right = 0;
 
// Variable for angular velocity measurement
float ang_velocity_right = 0;
float ang_velocity_right_deg = 0;
 
const float rpm_to_radians = 0.10471975512;
const float rad_to_deg = 57.29578;
float previous_ang_velocity;
float ang_accelration;
int pwm_signal = 380;

#include <Wire.h>        // IIC communication library
#include <PWM.h>        // Require Timer1 PWM frequency of 20Khz-25Khz for Nidec 24H677 BLDC Motor (https://github.com/terryjmyers/PWM)


//************** Nidec 24H677H010 BLDC Motor Vars ***********************
const int nidecBrake = 8;      // Brake
const int nidecDirection = 7;  // CW/CCW Nidec Motor
const int ledsDirection = 6;   // Green LED direction indicators
const int ledsBrake = 5;       // Red LED direction indicators
const int nidecPWM = 9;        // Nidec Motor PWM
//************** End of Nidec 24H677H010 BLDC Motor Vars ******************

int brake_trigger = 1;         //Brake is ON
float pwm = 0;                   //PWM signal set to 0
int dir = 0;                   //Direction set to CCW
  


void setup() {
 
  // Open the serial port at 9600 bps
  Serial.begin(9600); 
  // PWM.h Setup
  InitTimersSafe();
  // The Nidec 24H677 BLDC Motor requires a PWM frequency of 20KHz to 25KHz
  bool success = SetPinFrequencySafe(nidecPWM, 20000);
   //set the Nidec motor control pins 
  pinMode(nidecBrake, OUTPUT);
  pinMode(nidecDirection,OUTPUT);
  pinMode(ledsDirection,OUTPUT);
  pinMode(ledsBrake,OUTPUT);
  pinMode(nidecPWM,OUTPUT);
 
  
  // Set pin states of the encoder
  pinMode(ENC_IN_RIGHT_A , INPUT_PULLUP);
  pinMode(ENC_IN_RIGHT_B , INPUT);
 
  // Every time the pin goes high, this is a pulse
  attachInterrupt(digitalPinToInterrupt(ENC_IN_RIGHT_A), right_wheel_pulse, RISING);

  //driveMotor(0,1);
}
 
void loop() {
  // Record the time
  currentMillis = millis();
  
  
  
  // If one second has passed, print the number of pulses
  if (currentMillis - previousMillis > interval) {
    driveMotor(pwm_signal, 1);
    pwm_signal = pwm_signal-15;
    previousMillis = currentMillis;
    
    // Calculate revolutions per minute
    rpm_right = (float)(right_wheel_pulse_count * 60 / ENC_COUNT_REV);
    ang_velocity_right = rpm_right * rpm_to_radians;   
    ang_velocity_right_deg = ang_velocity_right * rad_to_deg;
    ang_accelration = (ang_velocity_right - previous_ang_velocity) / 1;
     
    //Serial.print("Pulses:");
    //Serial.print(right_wheel_pulse_count);
    //Serial.print("   ");
    
    //Serial.print("   ");
    //Serial.println();
 
    right_wheel_pulse_count = 0;
    previous_ang_velocity = ang_velocity_right;

  }
  Serial.print("RPM:");
  Serial.println(rpm_right);
  //Serial.print("   ");
  //Serial.print("AngularVelocity:");
  //Serial.println(ang_velocity_right);
  //Serial.print("   ");
  //Serial.print("AngularAccleration:");
  //Serial.println(ang_accelration);
  
  //Serial.print("pwm_signal:");
  //Serial.print(pwm_signal);
  //Serial.print("   ");
}
 
// Increment the number of pulses by 1
void right_wheel_pulse() {
   
  // Read the value for the encoder for the right wheel
  int val = digitalRead(ENC_IN_RIGHT_B);
 
  if(val == LOW) {
    Direction_right = false; // Reverse
  }
  else {
    Direction_right = true; // Forward
  }
   
  if (Direction_right) {
    right_wheel_pulse_count++;
  }
  else {
    right_wheel_pulse_count--;
  }
}

// For pwm: 400 = stopped and 0 = full speed
// For dir: 0 = CCW and 1 = CW
void driveMotor(float pwm, int dir){
  digitalWrite(nidecBrake,HIGH);    // Nidec motor brake OFF
  digitalWrite(ledsBrake,LOW);      //Red LED OFF // Brake OFF
  if(dir == 0){
    analogWrite(nidecPWM, pwm);
    digitalWrite(nidecDirection,LOW); // Nidec Direction CCW
    digitalWrite(ledsDirection,LOW);  //Green LED OFF // Direction CCW
    }
  else if(dir == 1){
    analogWrite(nidecPWM, pwm);
    digitalWrite(nidecDirection,HIGH); // Nidec Direction CW
    digitalWrite(ledsDirection,HIGH);  //Green LED ON // Direction CW
    } 
}
