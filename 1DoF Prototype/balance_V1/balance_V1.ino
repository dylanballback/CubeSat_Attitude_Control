
#include <Wire.h>
#include <Kalman.h> // Source: https://github.com/TKJElectronics/KalmanFilter
#include <Wire.h>        // IIC communication library
#include <PWM.h>        // Require Timer1 PWM frequency of 20Khz-25Khz for Nidec 24H677 BLDC Motor


//------------------------------------------------- Kalman Filter START ------------------------------------------------

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

//--------------------------------------------------- Kalman Filter END ---------------------------------------------------------------


//------------------------------------------ BLDC Motor Set up START -----------------------------------------------------------

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
  analogWrite(nidecPWM,0); // No PWM Signal

//---------------------------------------------- BLDC Motor Set up END ---------------------------------------------------------


////////////////////// Begin of PID parameters ///////////////////////////////
double kp = 28;
double ki = 0; // NOT USED
double kd = 0.62; 
////////////////////// End of PID parameters /////////////////////////////////

///////////////////////////////// Begin of PID speed loop Vars //////////////////////
double kp_speed =  3; 
double ki_speed = 0.072;
double kd_speed = 0; // NOT USED  
double targetAngle = -5; // Angle balance point
int PD_pwm;  //angle output
float pwmOut=0;
float pwmOut2=0; 
///////////////////////////////// End of PID speed loop Vars //////////////////////

//////////////////////////////// Begin of PI_pwm Vars //////////////////////////
float speeds_filterold=0;
float positions=0;
double PI_pwm;
int cc;
float speeds_filter;
//////////////////////////////// End of PI_pwm Vars /////////////////////////////


void setup() {
  
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

  timer = micros();
//------------------------------------------------ Kalman Filter END --------------------------------------------------------







}

void loop() {
  // put your main code here, to run repeatedly:

}








//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$  Kalman Filter Function START $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

double kal_x_angle() {
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
      
  Serial.print(kalAngleX);
  Serial.print('\n');
}

//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$  Kalman Filter Function END $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

////////////////// Angle PD_pwm ////////////////////
void PD()
{
  PD_pwm = kp * (angle + angle0) + kd * angle_speed; //PD angle loop control
}

////////////////// Begin of Speed PI_pwm ////////////////////
void SpeedPIout()
{
  float speeds = (rwPulse) * 1.0;      //speed  pulse value
  rwPulse = 0;
  speeds_filterold *= 0.7;         //first-order complementary filtering
  speeds_filter = speeds_filterold + speeds * 0.3;
  speeds_filterold = speeds_filter;
  positions += speeds_filter;
  positions = constrain(positions, -3550,3550);    //Anti-integral saturation
  PI_pwm = ki_speed * (targetAngle - positions) + kp_speed * (targetAngle - speeds_filter);      //speed loop control PI
}
////////////////// End of Speed PI_pwm ////////////////////

//////////////////////////// Final PWM Values /////////////////////////////
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
