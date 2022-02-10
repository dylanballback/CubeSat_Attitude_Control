

#include <MPU6050.h>
#include <Wire.h>       


//MPU 6050 
int16_t accX, accY, accZ, gyroX, gyroY, gyroZ; // 6DOF 3-axis acceleration and 3-axis gyroscope variables

float angle0 = -5; //mechanical balance angle (ideally 0 degrees) 
float Gyro_x;
float Gyro_y;
float Gyro_z;  //Angular velocity by gyroscope calculation

void setup() {
  // put your setup code here, to run once:
  // join I2C bus
  Wire.begin();                            
  delay(500);
  MPU6050.initialize(); //initialize MPU6050
  delay(500);
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  MPU6050.getMotion6(&accX, &accY, &accZ, &gyroX, &gyroY, &gyroZ);     //IIC to get MPU6050 six-axis data  ax ay az gx gy gz
  angle_calculate(accX, accY, accZ, gyroX, gyroY, gyroZ, dt, Q_angle, Q_gyro, R_angle, C_0, K1);      //get angle and Kalmam filtering

  ///////////////////////////// Tilt calculations ///////////////////////
void angle_calculate(int16_t accX,int16_t accY,int16_t accZ,int16_t gyroX,int16_t gyroY,int16_t gyyroZ,float dt,float Q_angle,float Q_gyro,float R_angle,float C_0,float K1)
{
  float Angle = -atan2(accY , accZ) * (180/ PI);           //Radial rotation angle calculation formula ; negative sign is direction processing
  Gyro_x = -gyroX / 131;              //The X-axis angular velocity calculated by the gyroscope;  the negative sign is the direction processing
  Kalman_Filter(Angle, Gyro_x);            //Kalman Filter
  //rotating angle Z-axis parameter
  Gyro_z = -gyroZ / 131;                      //angle speed of Z-axis
  //accelz = accZ / 1604;

  float angleAx = -atan2(accX, accZ) * (180 / PI); //calculate the inclined angle with x-axis
  Gyro_y = -gyroY / 131.00; //angle speed of Y-axis
  Yiorderfilter(angleAx, Gyro_y); //first-order filtering
}
////////////////////////////////////////////////////////////////

/////////////////////////////// Kalman Filter Calculations /////////////////////
void Kalman_Filter(double angle_m, double gyro_m)
{
  angle += (gyro_m - q_bias) * dt;          //prior estimate
  angle_err = angle_m - angle;
  
  Pdot[0] = Q_angle - P[0][1] - P[1][0];    //The differential of the covariance of the prior estimate error
  Pdot[1] = - P[1][1];
  Pdot[2] = - P[1][1];
  Pdot[3] = Q_gyro;
  
  P[0][0] += Pdot[0] * dt;    //The integral of the covariance differential of the prior estimate error
  P[0][1] += Pdot[1] * dt;
  P[1][0] += Pdot[2] * dt;
  P[1][1] += Pdot[3] * dt;
  
  //Intermediate variables in matrix multiplication 
  PCt_0 = C_0 * P[0][0];
  PCt_1 = C_0 * P[1][0];
  //denominator
  E = R_angle + C_0 * PCt_0;
  //gain value
  K_0 = PCt_0 / E;
  K_1 = PCt_1 / E;
  
  t_0 = PCt_0;  // Intermediate variables in matrix multiplication
  t_1 = C_0 * P[0][1];
  
  P[0][0] -= K_0 * t_0;    // Posterior estimation error covariance
  P[0][1] -= K_0 * t_1;
  P[1][0] -= K_1 * t_0;
  P[1][1] -= K_1 * t_1;
  
  q_bias += K_1 * angle_err;    // Posterior estimate
  angle_speed = gyro_m - q_bias;   // The differential of the output value gives the optimal angular velocity
  angle += K_0 * angle_err; // Posterior estimation; get the optimal angle
  Serial.print(angle)
}
  
}
