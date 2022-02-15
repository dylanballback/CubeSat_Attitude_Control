
//This code is written based off of https://github.com/squirtifier/DIY---1D-Wannabe-Cubli-Reaction-Wheel/blob/main/ReactionwheelV13.5Final/ReactionwheelV13.5/ReactionwheelV13.5.ino
//This is a simple test of spining a Nidec 24H404H BLDC Motor in the CC and CCW Direction through an Arduino

#include <Wire.h>        // IIC communication library
#include <PWM.h>        // Require Timer1 PWM frequency of 20Khz-25Khz for Nidec 24H677 BLDC Motor (https://github.com/terryjmyers/PWM)


//************** Nidec 24H677H010 BLDC Motor Vars ****************
const int nidecBrake = 8;      // Brake
const int nidecDirection = 7;  // CW/CCW Nidec Motor
const int ledsDirection = 6;   // Green LED direction indicators
const int ledsBrake = 5;       // Red LED direction indicators
const int nidecPWM = 9;        // Nidec Motor PWM
//************** End of Nidec 24H677H010 BLDC Motor Vars ****************


void setup() {
  Serial.begin(9600);
  //*************************** Nidec 24H677H010 BLDC Motor Setup *****************************
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
  
  // Output initial state values
  digitalWrite(nidecDirection,LOW); // Nidec Direction CCW
  digitalWrite(ledsDirection,LOW); //Green LED OFF // Direction CCW
  digitalWrite(nidecBrake,LOW); // Nidec motor brake is ON
  digitalWrite(ledsBrake,HIGH); //Red LED ON // Brake ON
  analogWrite(nidecPWM,400); // Motor Stop PWM Signal

}

// Nidec motor PWM - 5% Duty Cycle = High Speed and 90% Duty Cycle = Low Speed
//  This is oppisite of a standard DC motor therefore we have to map different values for pwmOut2
// pwmOut2 = map(pwmOut, -255, 255, -180, 130);//-130, 130 // modify the last two values for your BLDC motor
//Serial.println(pwmOut);
  
// Motor Full Speed at pwm = 0
// Motor Stop at pwm = 390

// Green LED is for Direction 
// CCW = Green LED OFF
// CW = Green LED ON

//Red LED is for Brake 
// Brake ON = Red LED ON
// Brake OFF = Red LED OFF

void loop() {
  digitalWrite(nidecBrake,HIGH);    // Nidec motor brake OFF
  digitalWrite(ledsBrake,LOW);      //Red LED OFF // Brake OFF
  digitalWrite(nidecDirection,LOW); // Nidec Direction CCW
  digitalWrite(ledsDirection,LOW); //Green LED OFF // Direction CCW
  analogWrite(nidecPWM, 100);
  delay(2000);
  //digitalWrite(nidecDirection,HIGH); // Nidec Direction CW
 // digitalWrite(ledsDirection,HIGH); //Green LED ON // Direction CW
  //analogWrite(nidecPWM, 1);
  //delay(2000);
  
/*
  //Clock Wise Test
  //Motor stopped gradually increasing to full speed
  for(int i=390; i>0; i--){
    analogWrite(nidecPWM, i);
    digitalWrite(nidecDirection,HIGH); // Nidec Direction CW
    digitalWrite(ledsDirection,HIGH);  //Green LED ON // Direction CW
    delay(10);
  }

  
  //Testing the Brake Feature
  analogWrite(nidecPWM,0); // No PWM Signal
  digitalWrite(nidecBrake,LOW);     // Nidec motor brake ON
  digitalWrite(ledsBrake,HIGH);     //Red LED ON // Brake ON
  delay(1500);
  digitalWrite(nidecBrake,HIGH);    // Nidec motor brake OFF
  digitalWrite(ledsBrake,LOW);      //Red LED OFF // Brake OFF


  //Motor full speed gradually decreasing to stopped
  for(int i=0; i<390; i++){
    analogWrite(nidecPWM, i);
    digitalWrite(nidecDirection,HIGH); // Nidec Direction CW
    digitalWrite(ledsDirection,HIGH);  //Green LED ON // Direction CW
    delay(10);
  }

  
  //Counter Clock Wise Test
  //Motor stopped gradually increasing to full speed
  for(int i=390; i>0; i--){
    analogWrite(nidecPWM, i);
    digitalWrite(nidecDirection,LOW); // Nidec Direction CCW
    digitalWrite(ledsDirection,LOW);  //Green LED OFF // Direction CCW
    delay(10);
  }
  
   //Motor full speed gradually decreasing to stopped
  for(int i=0; i<390; i++){
    analogWrite(nidecPWM, i);
    digitalWrite(nidecDirection,LOW); // Nidec Direction CCW
    digitalWrite(ledsDirection,LOW);  //Green LED OFF // Direction CCW
    delay(10);
  }
  */
 } 
