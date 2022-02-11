
#include <Kalman.h> // Source: https://github.com/TKJElectronics/KalmanFilter
#include <Wire.h>        // IIC communication library
#include <PWM.h>        // Require Timer1 PWM frequency of 20Khz-25Khz for Nidec 24H677 BLDC Motor

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
//---------------------------------------- Kalman Filter START -------------------------------------------------------

/*
float elapsedTime, time, timePrev; //time vars
//---------------------------------------- PID Vars START -------------------------------------------------------
//PID constants
double kp = 3; // Proportional contribution
double ki = 4; // Integral contribution
double kd = 0; // Derivative contribution

double error, lastError;                 // Initialize error and previousError
double input, output, setPoint;          // Initialize input variable from IMU, the output variable, and the desired setPoint
double cumError, rateError;              // Initialize the cumulative Error (Integral) and the rate of Error (Derivative)
//---------------------------------------- PID Vars END  -------------------------------------------------------
*/

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
int errorcw = 0;
int errorccw = 0;
int correction = 0;
double kp = 57;
double ki = 26;
double kd = 12;
int PID = 0;
int error = 0;
int last_error = 0;
int angle = 0;
int sp = 839;
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


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
  
  time = millis(); //Start counting time in milliseconds

}


//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%





void loop() {
  
  double x_angle = - kalman();
  Serial.print("x_angle: ");
  Serial.print(x_angle);
  Serial.print("               "); 

  
  /*
  if (x_angle > 4){
    dir = 0; //CCW
    Serial.print("dir: ");
    Serial.print("CW");
    Serial.print("               ");
  }
  else if (x_angle < 4){
    dir = 1; //CW
    Serial.print("dir: ");
    Serial.print("CCW");
    Serial.print("               ");
  }
  */

  if(x_angle < sp)
  {
  errorcw = (sp-x_angle);
  correction = calcPid(errorcw);
  dir = 1; //CW
  }
  if(x_angle > sp)
  {
  errorccw = (x_angle-sp);
  correction = calcPid(errorccw);
  dir = 0; //CCW
  }
  Serial.print("errorcw: ");
  Serial.print(errorcw);
  Serial.print("               ");
  Serial.print("correction: ");
  Serial.print(correction);
  Serial.print("               ");
  Serial.print("dir: ");
  Serial.print(dir);
  Serial.print("               ");
}//end of loop void



//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
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

// For pwm: 400 = stopped and 0 = full speed
// For dir: 0 = CCW and 1 = CW
void driveMotor(float pwm,int dir){
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

/*
// V2 PID function
double computePID()
{
    read_IMU();
    
    // Percentage
    volatile float IMU_deg_data_Z_perc = (IMU_deg_data_Z - 360)/(-360)*100;

    OBC_data_value = OBC_data_value + IMU_deg_data_Z;
    
    if (OBC_data_value < 0) OBC_data_value += 360;
    if (OBC_data_value >= 360) OBC_data_value -= 360; //Estas dos lineas contienen el valor del heading en 0-360 grados.
    
    // Transform angle to percentage
    volatile float OBC_data_value_perc = (OBC_data_value - 360)/(-360)*100;


    // Time
    currentTime = millis();                             // Get current time
    elapsedTime = (double)(currentTime - previousTime); // Compute elapsed time from previous time computation

    // Errors
    error = OBC_data_value_perc - IMU_deg_data_Z_perc;       // Calculate error (Proportional)
    cumError += error * elapsedTime;               // Calculate the cumulative error (Integral)
    rateError = (error - lastError) / elapsedTime; // Calculate the rate of error (Derivative)

    // PID Control
    PID_P = kp * error;     // Proportional
    PID_I = ki * cumError;  // Integral
    PID_D = kd * rateError; // Derivative

    if (PID_P > 400) PID_P = 400;
    if (PID_P < 0) PID_P = 0;
    if (PID_I > 400) PID_I = 400;
    if (PID_I < 0) PID_I = 0;
    if (PID_D > 400) PID_D = 400;
    if (PID_D < 0) PID_D = 0;

    double PID_output = PID_P + PID_I + PID_D; // PID control

    if (PID_output > 400) PID_output = 400;
    if (PID_output < 0) PID_output = 0;


    // Save current error and time for next iteration
    lastError = error;          // Save current error
    previousTime = currentTime; // Save current time

    // PID control return
    return PID_output;
}
*/

int calcPID(int error)
{
  PID = (kp*(error)) + (ki*(error + last_error)) + (kd*(error - last_error));
  last_error = error;
  return constrain(PID, 0, 400)
}
