#pragma once


void nano_imu_init()
{
  if (!IMU.begin()) 
  {
    Serial.println("Failed to initialize IMU!");

    while (1);
  }

  Serial.print("Gyroscope sample rate = ");
  Serial.print(IMU.gyroscopeSampleRate());
  Serial.println(" Hz");
  Serial.println();
  Serial.println("Gyroscope in degrees/second");
  Serial.println("X\tY\tZ");
}


float nano_gyro_data()
{
  float x, y, z;
  
  if (IMU.gyroscopeAvailable()) 
  {
    IMU.readGyroscope(x, y, z);
    
    Serial.print("Gyro: ");
    Serial.print(x);
    Serial.print('\t');
    Serial.print(y);
    Serial.print('\t');
    Serial.println(z);

    return x;
  }
}


int interval = 10;



float nano_acc_data()
{
  float x, y, z;
  if (IMU.accelerationAvailable()) 
  {
  IMU.readAcceleration(x, y, z);
  Serial.print("Acceleration: ");
  Serial.print(x);
  Serial.print('\t');
  Serial.print(y);
  Serial.print('\t');
  Serial.print(z);
  Serial.print('\t');
  }
}
