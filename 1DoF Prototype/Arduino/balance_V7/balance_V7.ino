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
//---------------------------------------- Nidec 24H677H010 BLDC Motor Vars END --------------------------------------


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

uint32_t timer;
uint8_t i2cData[14]; // Buffer for I2C data
//---------------------------------------- Kalman Filter END -------------------------------------------------------



//---------------------------------------- PID Vars START -------------------------------------------------------
//PID constants
double kp = 0;
double ki = .1;
double kd = 1;
/*
 * I had a bit of success with very low I values with some dampening, because the integral leads more directly to acceleration and thus torque, but I think there are some problems with the PID implementation
 * If my understanding of your code is correct, you're using the location of the wheel to set the direction of the motor, the direction of motion for a PID is supposed to be set by the sign of the output
 * My recommendation would be to have your motor driving function take in a value where 0 is stationary, and the direction is the sign
 * Not sure how much that bit will help immedeately, but I did notice some weird behavior, and that might be the cause
 * The second thing is you'll probably want to have the output of the PID control the acceleration of the wheel, and thus torque, rather than the velocity
 * In the current implementation, without an integral component, the wheel will have no torque applied to it when held off the stationary point, as a constant angular velocity applies no torque (or very little)
 * 
 * Lastly, be careful of the accelerometer calibration of the mpu6050s, it may not be reporting a correct angle. You might just want to use one axis of the accelerometer, record its highest value and use trig to compute an angle off of it
 * 
 * Hopefully some of this is helpful, I'm not an expert though
 * 
 * Good luck,
 * -Ryan
 */
 
unsigned long currentTime, previousTime;
double elapsedTime;
double error;
double lastError;
double Input, Output, Setpoint;
double i, d;
double outMin = 0;
double outMax = 390;
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
}
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% Setup Function END %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% Loop Function START %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
void loop() {
  Serial.print("SetPoint:");
  Serial.print(Setpoint);
  Serial.print("   ");
  //Serial.print("outMin:");
  //Serial.print(outMin);
  //Serial.print("   ");
  //Serial.print("outMax:");
  //Serial.print(outMax);
  //Serial.print("   ");
  
  double x_angle = kalman();
  Serial.print("x_angle:");
  Serial.println(x_angle);
  //Serial.print("   ");

  
  Input = x_angle;
  if (Input > 0){
    Input = -Input;
  }
  //Serial.print("Input:");
  //Serial.print(Input);
  //Serial.print("   ");
  
  //Switches the direction of the wheel based on angle
  if (x_angle > 1){
    dir = 0; //CCW
    //Serial.print("dir: ");
    //Serial.print("CW");
    //Serial.print("    ");
  }
  else if (x_angle < 0){
    dir = 1; //CW
    //Serial.print("dir: ");
    //Serial.print("CCW");
    //Serial.print("    ");
  }
  
  //Calculate the PID output
  Output = computePID(Input);
  
  

  
  
  //This is just fliping the PWM value from 0 to 390 ---> to 390 to 0
  // This is to make the PWM value make more sense while graphing
  //Whereas 0 = stopped and 390 = full speed
  double pwm = map(Output, 0, 390, 390, 0);
  //Serial.print("pwm:");
  //Serial.println(pwm);
  //Serial.print("   "); 
  //Serial.print("\n");
  
  //This is the comand to drive the motor
  driveMotor(pwm, dir);
  

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
  return kalAngleX;
}
//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$  Kalman Filter Function END $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$



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
double computePID(double inp){
  currentTime = millis();                //get current time
  elapsedTime = (double)(currentTime - previousTime);        //compute time elapsed from previous computation
  
  error = Setpoint - inp;    // determine error
  //Serial.print("error:");
  //Serial.print(error);
  //Serial.print("   ");
  i += error * elapsedTime;  // compute integral
  if(i> outMax){
    i=outMax;
  }
  else if(i< outMin){
    i=outMin;
  }
  //Serial.print("i:");
  //Serial.print(i);
  //Serial.print("   ");
  d = (error - lastError)/elapsedTime;   // compute derivative
  //Serial.print("d:");
  //Serial.print(d);
  //Serial.print("   ");
  double Output = kp*error + ki*i + kd*d;  //PID output               
  if(Output > outMax){ 
    Output = outMax;
  }
  else if(Output < outMin){
    Output = outMin;
  }
  //Serial.print("Output:");
 // Serial.print(Output);
  //Serial.print("   ");
  lastError = error;            //remember current error
  previousTime = currentTime;   //remember current time
          
  return Output;   //have function return the PID output
}



//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ Compute PID Function END $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
