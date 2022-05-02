#pragma once

#define SERIESRESISTOR 10000 

#define VOLTAGE_INTERVAL 5000
unsigned long lastExecutedMillis_voltage = 0;
#include "SensorData8.h"
SensorData8 ssensor_data;

int ThermistorPin = A7;
int Vo;
float R1 = 10000;
float logR2, R2, T, Tf;
float a;
float c1 = 1.009249522e-03, c2 = 2.378405444e-04, c3 = 2.019202697e-07;
  

void thermister_read() 
{
  // Explain how this function does and how 
  Vo = analogRead(ThermistorPin);
  R2 = R1 * (1023.0 / (float)Vo - 1.0);
  logR2 = log(R2);
  T = (1.0 / (c1 + c2*logR2 + c3*logR2*logR2*logR2));
  a = T - 273.15;
  Tf = (a * 9.0)/ 5.0 + 32.0;
  ssensor_data.tempp = a;
};

void print_therm()
{
  Serial.print("Temp: ");
  Serial.println(ssensor_data.tempp);
};


void read_photo()
{
  float photor1 = analogRead(A3);
  float photor4 = analogRead(A6);

  float pr1 = photor1 * (3.3 / 1023.0);
  float pr4 = photor4 * (3.3 / 1023.0);

  ssensor_data.photo1 = map(pr1, 0.8, 2.8, 0, 50);
  ssensor_data.photo2 = map(pr4, 0.8, 2.8, 0, 50);

};

void print_photos()
{
  Serial.print("Photo 1: ");
  Serial.print(ssensor_data.photo1);
  Serial.print("    ");
  Serial.print("Photo 4: ");
  Serial.println(ssensor_data.photo2);
};



void read_voltage()
{
  int c1 = analogRead(A0);
  int c2 = analogRead(A1);
  int c3 = analogRead(A2);
  
  float v1 = c1 * (3.3 / 1023.0);
  float v2 = c2 * (3.3 / 1023.0);
  float v3 = c3 * (3.3 / 1023.0);
  
  ssensor_data.cell1v = v1;
  ssensor_data.cell2v = v2;
  ssensor_data.cell3v = v3;
};


void print_voltages()
{
  Serial.print("Cell 1 Voltage: ");
  Serial.print(ssensor_data.cell1v);
  Serial.print("    ");
  Serial.print("Cell 2 Voltage: ");
  Serial.print(ssensor_data.cell2v);
  Serial.print("    ");
  Serial.print("Cell 3 Voltage: ");
  Serial.println(ssensor_data.cell3v);
};
