#include <Kalman.h> // Source: https://github.com/TKJElectronics/KalmanFilter
#include <Wire.h>        // IIC communication library
#include <PWM.h>        // Require Timer1 PWM frequency of 20Khz-25Khz for Nidec 24H677 BLDC Motor


//Cite http://brettbeauregard.com/blog/2011/04/improving-the-beginner%E2%80%99s-pid-reset-windup/)

//---------------------------------------- Nidec 24H677H010 BLDC Motor Vars START ------------------------------------
const int nidecBrake = 8;      // Brake
const int nidecDirection = 7;  // CW/CCW Nidec Motor
const int ledsDirection = 6;   // Green LED direction indicators
const int ledsBrake = 5;       // Red LED direction indicators
const int nidecPWM = 9;        // Nidec Motor PWM
int brake_trigger = 1;         //Brake is ON
int dir = 0;                   //Direction set to CCW
double pwm = 400;              // Set pwm value to 400 = motor stopped
//---------------------------------------- Nidec 24H677H010 BLDC Motor Vars END --------------------------------------





//---------------------------------------- Encoder Vars START ------------------------------------
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
int interval = 100;
  
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
//---------------------------------------- Encoder Vars END ------------------------------------




//---------------------------------------- Kalman Filter START -------------------------------------------------------
#define RESTRICT_PITCH // Comment out to restrict roll to ±90deg instead - please read: http://www.freescale.com/files/sensors/doc/app_note/AN3461.pdf

Kalman kalmanX; // Create the Kalman instances
Kalman kalmanY;

/* IMU Data */
double accX, accY, accZ;
double gyroX, gyroY, gyroZ;
int16_t tempRaw;

double gyroXangle, gyroYangle; // Angle calculate using the gyro only
double compAngleX, compAngleY; // Calculated angle using a complementary filter
double kalAngleX, kalAngleY; // Calculated angle using a Kalman filter
double rad_kalAngleX;

uint32_t timer;
uint8_t i2cData[14]; // Buffer for I2C data
//---------------------------------------- Kalman Filter END -------------------------------------------------------



//---------------------------------------- PID Vars START -------------------------------------------------------
//PID constants
double kp = 1; //*57.295 converts from rad/s to deg/s
double ki = 0;
double kd = 0.5; //*57.295 converts from rad/s to deg/s

double outMin = -8;
double outMax = 8;

//Motor Back EMF Constant
double Kw = 0.00834;
int max_motor_V = 24; 

double bounds = 0.2;
int term;
 
unsigned long currentTime, previousTime;
double elapsedTime;
double error;
double lastError;
double Input, Output, Setpoint;
double i, d;
double pwm_out;
double Output_pwm;

//---------------------------------------- PID Vars END -----------------------------------------------------------



//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% Setup Function Start %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
void setup() {
  
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

//---------------------------------------- Encoder START -------------------------------------------------------
  // Set pin states of the encoder
  pinMode(ENC_IN_RIGHT_A , INPUT_PULLUP);
  pinMode(ENC_IN_RIGHT_B , INPUT);
 
  // Every time the pin goes high, this is a pulse
  attachInterrupt(digitalPinToInterrupt(ENC_IN_RIGHT_A), right_wheel_pulse, RISING);
//---------------------------------------- Encoder End -------------------------------------------------------

  
//---------------------------------------- Kalman Filter START -------------------------------------------------------
  Serial.begin(115200);
  Wire.begin();
  #if ARDUINO >= 157
    Wire.setClock(400000UL); // Set I2C frequency to 400kHz
  #else
    TWBR = ((F_CPU / 400000UL) - 16) / 2; // Set I2C frequency to 400kHz
  #endif

  i2cData[0] = 7; // Set the sample rate to 1000Hz - 8kHz/(7+1) = 1000Hz
  i2cData[1] = 0x00; // Disable FSYNC and set 260 Hz Acc filtering, 256 Hz Gyro filtering, 8 KHz sampling
  i2cData[2] = 0x00; // Set Gyro Full Scale Range to ±250deg/s
  i2cData[3] = 0x00; // Set Accelerometer Full Scale Range to ±2g
  while (i2cWrite(0x19, i2cData, 4, false)); // Write to all four registers at once
  while (i2cWrite(0x6B, 0x01, true)); // PLL with X axis gyroscope reference and disable sleep mode

  while (i2cRead(0x75, i2cData, 1));
  if (i2cData[0] != 0x68) { // Read "WHO_AM_I" register
    Serial.print(F("Error reading sensor"));
    while (1);
  }

  delay(100); // Wait for sensor to stabilize

  /* Set kalman and gyro starting angle */
  while (i2cRead(0x3B, i2cData, 6));
  accX = (int16_t)((i2cData[0] << 8) | i2cData[1]);
  accY = (int16_t)((i2cData[2] << 8) | i2cData[3]);
  accZ = (int16_t)((i2cData[4] << 8) | i2cData[5]);

  // Source: http://www.freescale.com/files/sensors/doc/app_note/AN3461.pdf eq. 25 and eq. 26
  // atan2 outputs the value of -π to π (radians) - see http://en.wikipedia.org/wiki/Atan2
  // It is then converted from radians to degrees
  #ifdef RESTRICT_PITCH // Eq. 25 and 26
    double roll  = atan2(accY, accZ) * RAD_TO_DEG;
    double pitch = atan(-accX / sqrt(accY * accY + accZ * accZ)) * RAD_TO_DEG;
  #else // Eq. 28 and 29
    double roll  = atan(accY / sqrt(accX * accX + accZ * accZ)) * RAD_TO_DEG;
    double pitch = atan2(-accX, accZ) * RAD_TO_DEG;
  #endif

  kalmanX.setAngle(roll); // Set starting angle
  kalmanY.setAngle(pitch);
  gyroXangle = roll;
  gyroYangle = pitch;
  compAngleX = roll;
  compAngleY = pitch;
//------------------------------------------------ Kalman Filter END --------------------------------------------------------
  
  Setpoint = 0;                          //set point at zero degrees
  Serial.print(Setpoint);
}
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% Setup Function END %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% Loop Function START %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
void loop() {
  //Using encoder to calculate RPM of motor
  // Record the time
  currentMillis = millis();
  // If one second has passed, print the number of pulses
  if (currentMillis - previousMillis > interval) {
    previousMillis = currentMillis;
    // Calculate revolutions per minute
    rpm_right = (float)(right_wheel_pulse_count * 60 / ENC_COUNT_REV);
    ang_velocity_right = rpm_right * rpm_to_radians;   
    ang_velocity_right_deg = ang_velocity_right * rad_to_deg;
    ang_accelration = (ang_velocity_right - previous_ang_velocity) / 1;
    right_wheel_pulse_count = 0;
    previous_ang_velocity = ang_velocity_right;
  }
  Serial.print("ang_velocity_right: ");
  Serial.print(ang_velocity_right);
  Serial.print("   ");

  double x_angle = kalman();

  Input = x_angle;

  
  //Calculate the PID output
  Output = computePID(Input, x_angle);

  
  
    //Switches the direction of the wheel based on sign of PID output
  if (Output > 1){
    dir = 1; //CW
    Output_pwm = convert_to_pwm(Output, rpm_right);
    //This is just fliping the PWM value from 0 to 390 ---> to 390 to 0
    //Whereas 0 = stopped and 390 = full speed
    pwm = map(Output_pwm, 0, 400, 400, 0);
    //Serial.print("dir: ");
    //Serial.print("CW");
    //Serial.print("    ");
  }
  else if (Output < 0){
    dir = 0; //CCW
    Output = abs(Output);
    Output_pwm = convert_to_pwm(Output, rpm_right);
    //This is just fliping the PWM value from -390 to 0 ---> to 390 to 0
    //Whereas 0 = stopped and 390 = full speed
    pwm = map(Output_pwm, 0, 400, 400, 0);
    //Serial.print("dir: ");
    //Serial.print("CCW");
    //Serial.print("    ");
  }


  if (x_angle > -bounds && x_angle < bounds){
    pwm = 400;
  }
  
  Serial.print("x_angle:");
  Serial.print(x_angle);
  Serial.print("   "); 
  Serial.print("Output:");
  Serial.print(Output);
  Serial.print("   "); 
  Serial.print("pwm:");
  Serial.println(pwm);

  //This is the comand to drive the motor
  driveMotor(pwm , dir);
  

}//end of loop void
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% Loop Function END %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%



//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$  Kalman Filter Function START $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
double kalman(){
  /* Update all the values */
  while (i2cRead(0x3B, i2cData, 14));
  accX = (int16_t)((i2cData[0] << 8) | i2cData[1]);
  accY = (int16_t)((i2cData[2] << 8) | i2cData[3]);
  accZ = (int16_t)((i2cData[4] << 8) | i2cData[5]);
  tempRaw = (int16_t)((i2cData[6] << 8) | i2cData[7]);
  gyroX = (int16_t)((i2cData[8] << 8) | i2cData[9]);
  gyroY = (int16_t)((i2cData[10] << 8) | i2cData[11]);
  gyroZ = (int16_t)((i2cData[12] << 8) | i2cData[13]);;

  double dt = (double)(micros() - timer) / 1000000; // Calculate delta time
  timer = micros();

  // Source: http://www.freescale.com/files/sensors/doc/app_note/AN3461.pdf eq. 25 and eq. 26
  // atan2 outputs the value of -π to π (radians) - see http://en.wikipedia.org/wiki/Atan2
  // It is then converted from radians to degrees
#ifdef RESTRICT_PITCH // Eq. 25 and 26
  double roll  = atan2(accY, accZ) * RAD_TO_DEG;
  double pitch = atan(-accX / sqrt(accY * accY + accZ * accZ)) * RAD_TO_DEG;
#else // Eq. 28 and 29
  double roll  = atan(accY / sqrt(accX * accX + accZ * accZ)) * RAD_TO_DEG;
  double pitch = atan2(-accX, accZ) * RAD_TO_DEG;
#endif

  double gyroXrate = gyroX / 131.0; // Convert to deg/s
  double gyroYrate = gyroY / 131.0; // Convert to deg/s

#ifdef RESTRICT_PITCH
  // This fixes the transition problem when the accelerometer angle jumps between -180 and 180 degrees
  if ((roll < -90 && kalAngleX > 90) || (roll > 90 && kalAngleX < -90)) {
    kalmanX.setAngle(roll);
    compAngleX = roll;
    kalAngleX = roll;
    gyroXangle = roll;
  } else
    kalAngleX = kalmanX.getAngle(roll, gyroXrate, dt); // Calculate the angle using a Kalman filter

  if (abs(kalAngleX) > 90)
    gyroYrate = -gyroYrate; // Invert rate, so it fits the restriced accelerometer reading
  kalAngleY = kalmanY.getAngle(pitch, gyroYrate, dt);
#else
  // This fixes the transition problem when the accelerometer angle jumps between -180 and 180 degrees
  if ((pitch < -90 && kalAngleY > 90) || (pitch > 90 && kalAngleY < -90)) {
    kalmanY.setAngle(pitch);
    compAngleY = pitch;
    kalAngleY = pitch;
    gyroYangle = pitch;
  } else
    kalAngleY = kalmanY.getAngle(pitch, gyroYrate, dt); // Calculate the angle using a Kalman filter

  if (abs(kalAngleY) > 90)
    gyroXrate = -gyroXrate; // Invert rate, so it fits the restriced accelerometer reading
  kalAngleX = kalmanX.getAngle(roll, gyroXrate, dt); // Calculate the angle using a Kalman filter
#endif

  gyroXangle += gyroXrate * dt; // Calculate gyro angle without any filter
  gyroYangle += gyroYrate * dt;
  //gyroXangle += kalmanX.getRate() * dt; // Calculate gyro angle using the unbiased rate
  //gyroYangle += kalmanY.getRate() * dt;

  compAngleX = 0.93 * (compAngleX + gyroXrate * dt) + 0.07 * roll; // Calculate the angle using a Complimentary filter
  compAngleY = 0.93 * (compAngleY + gyroYrate * dt) + 0.07 * pitch;

  // Reset the gyro angle when it has drifted too much
  if (gyroXangle < -180 || gyroXangle > 180)
    gyroXangle = kalAngleX;
  if (gyroYangle < -180 || gyroYangle > 180)
    gyroYangle = kalAngleY;
      
  //Serial.print(kalAngleX);
  //Serial.print('\n');
  //return kalAngleX;

  

  rad_kalAngleX = deg_to_rad(kalAngleX);
  //Serial.println(rad_kalAngleX);
  return rad_kalAngleX;
  
}
//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$  Kalman Filter Function END $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

// converts degrees to radians 
double deg_to_rad(double deg_in) {
  double rad_out = deg_in * 0.174532925;
  return rad_out;
}


//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$  Encoder Function START $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
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
//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$  Encoder Function END $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$



//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$  Drive Motor Function START $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
// For pwm: 400 = stopped and 0 = full speed
// For dir: 0 = CCW and 1 = CW
void driveMotor(float pwm,int dir){
  //if pwm 
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
//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$  Drive Motor Function END $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$


//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ Compute PID Function START $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
double computePID(double inp, double x_angle){
  currentTime = millis();                //get current time
  elapsedTime = (double)(currentTime - previousTime);        //compute time elapsed from previous computation
  
  error = Setpoint - inp;    // determine error
  
  
  i += error * elapsedTime;  // compute integral
  if(i> outMax){
    i=outMax;
  }
  else if(i< outMin){
    i=outMin;
  }
 
  d = (error - lastError)/elapsedTime;   // compute derivative

  double Output = kp*error + ki*i + kd*d;  //PID output
                
  if(Output > outMax){ 
    Output = outMax;
  }
  else if(Output < outMin){
    Output = outMin;
  }
  
  lastError = error;            //remember current error
  previousTime = currentTime;   //remember current time
          
  return Output;   //have function return the PID output
}

double convert_to_pwm(double pid_out, double wheel_vel){
  double v_mot = Kw*wheel_vel;
  double v_sup = 3*pid_out+v_mot;
  pwm_out = map(v_sup, 0, 24, 0, 400);
  //double x = (3*pid_out+Kw*wheel_vel);
  //pwm_out = 400*(1-x/max_motor_V);
  return pwm_out;
}

//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ Compute PID Function END $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
