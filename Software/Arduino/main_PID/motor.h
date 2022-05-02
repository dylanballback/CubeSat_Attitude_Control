#pragma once

//const int nidecBrake = 8;      // Brake
//const int nidecDirection = 7;  // CW/CCW Nidec Motor
//const int nidecPWM = 6;        // Nidec Motor PWM

class Motor
{
  private:
    byte nidecBrake;
    byte nidecDirection; 
    byte nidecPWM;
    byte BrakeLED;
    byte DirectionLED;
  
  public:
    void set_LEDs(byte brake_pin, byte dir_pin);
    void set_nidecBrake(byte pin);
    void set_nidecDirection(byte pin);
    void set_nidecPWM(byte pin);
    void motor_setup();
    void set_motor_speed(int pwmDC);
    void set_motor_dir(int dir);
    void set_motor_dir_speed(int dir, int pwmDC);
    void set_brake(int brake_trigger);
};

void Motor::set_nidecBrake(byte pin)
{
  nidecBrake = pin;
};

void Motor::set_nidecDirection(byte pin)
{
  nidecDirection = pin;
};

void Motor::set_nidecPWM(byte pin)
{
  nidecPWM = pin;
};

void Motor::set_LEDs(byte brake_pin, byte dir_pin)
{
  BrakeLED = brake_pin;
  DirectionLED = dir_pin;
};


//Initalizes the motor pins and PWM frequency
void Motor::motor_setup()
{
  //set the Nidec motor control pins 
  pinMode(nidecBrake, OUTPUT);
  pinMode(nidecDirection, OUTPUT);
  pinMode(nidecPWM, OUTPUT);
  // Output initial state values
  //motor_dir(nidecDirection, 0);
  digitalWrite(nidecDirection, LOW); // Nidec Direction CCW
  digitalWrite(nidecBrake, HIGH); // Nidec motor brake is OFF
  
  // Input clock is divided by 1 and sent to Generic Clock, Turbo is Off
  pwm.setClockDivider(1, false); 
  
  // Timer 2 is set to Generic Clock divided by 1,
  // resolution is 600, phase-correct aka dual-slope PWM
  pwm.timer(0, 2, 600, false);     
};


//Motor variaies in speed from 
//Full speed  =>  pwmDC = 0
//Stopped     =>  pwmDC = 1000
void  Motor::set_motor_speed(int pwmDC)
{
  pwm.analogWrite(nidecPWM, pwmDC); //Nidec speed
};


//Motor Direction  => CCW =>  dir = 0
//Motor Direction  => CW  =>  dir = 1
void Motor::set_motor_dir(int dir)
{
  digitalWrite(nidecDirection, dir); // Nidec Direction
};


//Set motor direction, speed
void Motor::set_motor_dir_speed(int dir, int pwmDC)
{
  digitalWrite(nidecDirection, dir); // Nidec Direction 
  pwm.analogWrite(nidecPWM, pwmDC);  //Nidec speed
};


//Brake On  =>  brake_trigger = 0
//Brake Off =>  brake_trigger = 1
void Motor::set_brake(int brake_trigger)
{
  digitalWrite(nidecBrake, brake_trigger); //Set brake 
};
    
