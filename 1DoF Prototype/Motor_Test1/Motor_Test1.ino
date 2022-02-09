

#include <MsTimer2.h>       // Internal Timer2 for ISR Routine
#include <Wire.h>        // IIC communication library
#include <PWM.h>        // Require Timer1 PWM frequency of 20Khz-25Khz for Nidec 24H677 BLDC Motor


//************** Nidec 24H677H010 BLDC Motor Vars ****************
const int nidecBrake = 8;      // Brake
const int nidecDirection = 7; // CW/CCW Nidec Motor
const int ledsDirection = 6; // Green/Red LED direction indicators
const int nidecPWM = 9;     // Nidec Motor PWM
float loopOnce=0; // Used to reset Nidec jump-up routine
//************** End of Nidec 24H677H010 BLDC Motor Vars ****************


////////////////// Begin of ISR speed count var /////////////////////////////
#define nidecHalleffect 12  // External interrupt D12 for Nidec FG output 6 Pulses/Revoltion
volatile long countHall = 0;// Used to calculate the pulse value calculated by the Hall encoder 
////////////////// End of ISR speed count var ///////////////////////////////


////////////////////// Begin of pulse count /////////////////////////
int rw = 0;
int pulseCount = 0;
int rwPulse;
////////////////////// End of pulse count //////////////////////////


//////////////////////////////// Begin of PI_pwm Vars //////////////////////////
float speeds_filterold=0;
float positions=0;
double PI_pwm;
int cc;
float speeds_filter;
//////////////////////////////// End of PI_pwm Vars /////////////////////////////


int loopcount; // Counter var for D13 onboard LED flash evey 1 second




void setup() {
  
  // PWM.h Setup
  InitTimersSafe();
  // The Nidec 24H677 BLDC Motor requires a PWM frequency of 20KHz to 25KHz
  bool success = SetPinFrequencySafe(nidecPWM, 20000);

  //set the Nidec motor control pins
  pinMode(nidecDirection,OUTPUT);
  pinMode(ledsDirection,OUTPUT);
  pinMode(nidecPWM,OUTPUT);
  pinMode(nidecBrake, OUTPUT);
  pinMode(nidecHalleffect, INPUT);  //speed encoder input
  pinMode(13, OUTPUT);  //D13 LED use scope to measure ISR routine to make sure interrupt is working
  // sample rate or (dt) = (1/100Hz) /2) = 5ms = 0.005s // You have to remove the for/nextloop for the LED blink routine

MsTimer2::set(5, ISR_Routine);
MsTimer2::start();    //start interrupt
 
}



void loop() 
  


void ReactionWheelPWM()
  {
  pwmOut=-PD_pwm; - PI_pwm;           //assign the end value of PWM to motor
  pwmOut = constrain(pwmOut, -255, 255);
  // Nidec motor PWM - 5% Duty Cycle = High Speed and 90% Duty Cycle = Low Speed
  //  This is oppisite of a standard DC motor therefore we have to map different values for pwmOut2
  pwmOut2 = map(pwmOut, -255, 255, -180, 130);//-130, 130 // modify the last two values for your BLDC motor
 //Serial.println(pwmOut);

  
  if (angle >= 25 && loopOnce == 1) // Reaction Wheel Right Side Jump-up Position 
   {
    for (int i = 0; i <= 10; i++) // A blinky delay 
     {
       delay(200);
       digitalWrite(ledsDirection,LOW);  // Green LED ON
       delay(200);
       digitalWrite(ledsDirection,HIGH);  // Red LED ON
     }
    digitalWrite(nidecBrake,HIGH);    // Nidec motor brake OFF 
    digitalWrite(nidecDirection,LOW); // Nidec Direction CW
    analogWrite(nidecPWM,185);
    delay(4000);
    digitalWrite(nidecDirection,HIGH); // Nidec Direction CCW
    analogWrite(nidecPWM,10);
    pwmOut2 = 0;
    delay(125);
    loopOnce = 0;
   } 

   if(angle <= -25 && loopOnce == 0) { // Reaction Wheel Left Side before moving to jump-up position
     for (int i = 0; i <= 20; i++) // Blinky countdown timer before reaction wheel spins
     {
     delay(50);
     digitalWrite(ledsDirection,HIGH);  // Red LED ON
     delay(50);
     digitalWrite(ledsDirection,LOW);  // Green LED ON
     }
     loopOnce = 1;
     }
  
 
  if(angle >= 20 || angle <= -20)  // if angle is greater than +/- 20° than the Nidec motor will stop
      {                                      
      digitalWrite(nidecBrake,LOW);  // Nidec motor brake ON
      analogWrite(nidecPWM, 0);
      }
      else
      {
          if(pwmOut>=0)         // Reaction wheel leaning to the left from center
          {
          digitalWrite(nidecBrake,HIGH);    // Nidec motor brake OFF 
          digitalWrite(nidecDirection,LOW); // Nidec Direction CW
          digitalWrite(ledsDirection,LOW);  // Green LED ON
          analogWrite(nidecPWM,pwmOut2);        // Nidec PWM speed 
          }
          else // Reaction wheel leaning to the right from center
          {
          digitalWrite(nidecBrake,HIGH);     // Nidec motor brake OFF 
          digitalWrite(nidecDirection,HIGH); // Nidec Direction CCW
          digitalWrite(ledsDirection,HIGH);  // Red LED ON
          analogWrite(nidecPWM, -pwmOut2);       // Nidec PWM speed  
          }
       } 
}




}
