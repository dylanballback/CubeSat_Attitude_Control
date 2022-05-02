#pragma once

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
//int interval =800;
int second = 1000/interval;
int minute = 60000/interval;
  
// Counters for milliseconds during interval
long previousMillis = 0;
long currentMillis = 0;
 
// Variable for RPM measuerment
float rpm_right = 0;
 
// Variable for angular velocity measurement
float ang_velocity_right = 0;
float prev_ang_velocity_right = 0;
float ang_velocity_right_deg = 0;
float ang_acc = 0;
 
const float rpm_to_radians = 0.10471975512;
const float rad_to_deg = 57.29578;

 
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

void init_encoder()
{
  // Set pin states of the encoder
  pinMode(ENC_IN_RIGHT_A , INPUT_PULLUP);
  pinMode(ENC_IN_RIGHT_B , INPUT);
 
  // Every time the pin goes high, this is a pulse
  attachInterrupt(digitalPinToInterrupt(ENC_IN_RIGHT_A), right_wheel_pulse, RISING);

}


void get_encoder()
{

  // If one second has passed, print the number of pulses
  if (currentMillis - previousMillis > interval) {
 
    previousMillis = currentMillis;
 
    // Calculate revolutions per minute
    rpm_right = (float)(right_wheel_pulse_count * minute) / ENC_COUNT_REV;
    ang_velocity_right = rpm_right * rpm_to_radians;   
    ang_velocity_right_deg = ang_velocity_right * rad_to_deg;
    ang_acc = (ang_velocity_right - prev_ang_velocity_right)/(second);
    prev_ang_velocity_right = ang_velocity_right;
    
    //Serial.println();
    right_wheel_pulse_count = 0;
 
    
  }  
}


void print_encoder()
{
    Serial.print("Pulses: ");
    Serial.print(right_wheel_pulse_count);
    Serial.print("\t");
    Serial.print("RPM: ");
    Serial.print(rpm_right);
    Serial.print("\t");
    Serial.print("Angular Velocity: ");
    Serial.print(ang_velocity_right);
    Serial.print("\t");
    Serial.print("rad/s");
    Serial.print("\t");
    Serial.print("Angular Acceleration: ");
    Serial.print(ang_acc);
    Serial.print("\t");
    Serial.println("rad/s^2");
}
