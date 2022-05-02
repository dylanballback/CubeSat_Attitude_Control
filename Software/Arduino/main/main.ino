#include "includes.h"

int old_i;

Motor xmotor;


void setup() 
{
  Serial.begin(9600);
  Wire.begin();
  byte status = mpu.begin();
  delay(1000);
  mpu.calcOffsets();


  // Motor pin setup.
  xmotor.set_nidecBrake(8);
  xmotor.set_nidecDirection(7); 
  xmotor.set_nidecPWM(6);

  //Motor set up function
  xmotor.motor_setup(); 

  //Websocket set up function
  websocket_init(); 
  xmotor.set_motor_dir_speed(1, 1000);
 
  Serial.print("Setup Function Complete");
}


void loop() 
{
  currentMillis = millis();
  mpu.update();
  float current_heading = mpu.getAngleZ();

  thermister_read();
  read_photo();
  read_voltage();
  

  //print_voltages();
  //print_photos();
  //print_therm();
  
  char buffer1[200];
  sprintf(buffer1, "accelerationData:%f:%f:%f", current_heading, ssensor_data.photo1, ssensor_data.photo2);
  sendMessage(buffer1);
  
  char buffer2[200];
  sprintf(buffer2, "BatteryData:%f:%f:%f:%f", ssensor_data.cell1v, ssensor_data.cell2v, ssensor_data.cell3v, ssensor_data.tempp);
  sendMessage(buffer2);

  
  
//  int i = get_data();
//  if (i != old_i)
//  {
//    old_i = i;
//    Serial.println(i);
//    //xmotor.set_motor_dir_speed(1, i);
//  }
  
}
  
