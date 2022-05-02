//#pragma once
//
//#define VOLTAGE_INTERVAL 5000
//unsigned long lastExecutedMillis_voltage = 0;
//
//
//SENSOR8 sensor_data;
//
//void read_voltage()
//{
//if (currentMillis - lastExecutedMillis_voltage >= VOLTAGE_INTERVAL)
//  {
//    lastExecutedMillis_voltage = currentMillis;
//    int c1 = analogRead(A0);
//    int c2 = analogRead(A1);
//    int c3 = analogRead(A2);
//    
//    float v1 = c1 * (3.3 / 1023.0);
//    float v2 = c2 * (3.3 / 1023.0);
//    float v3 = c3 * (3.3 / 1023.0);
//
//    sensor_data.cell1v = v1;
//    sensor_data.cell2v = v2;
//    sensor_data.cell3v = v3;
//    
//  }
//};
//
//
//void print_voltages()
//{
//  Serial.print("Cell 1 Voltage: ");
//  Serial.print(v1);
//  Serial.print("    ");
//  Serial.print("Cell 2 Voltage: ");
//  Serial.print(v2);
//  Serial.print("    ");
//  Serial.print("Cell 3 Voltage: ");
//  Serial.println(v3);
//}
