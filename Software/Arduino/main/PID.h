#pragma once

//PID constants
double kp = 10;
double ki = 0;
double kd = 0;

int Setpoint = 0;

double outMin = -400;
double outMax = 400;

double bounds = 0;
int term;
 
unsigned long currentTime, previousTime;
double elapsedTime;
double error;
double lastError;
double Input, Output;
double i, d;


double computePID(double inp, int setpoint){
  currentTime = millis();                //get current time
  elapsedTime = (double)(currentTime - previousTime);        //compute time elapsed from previous computation
  
  error = setpoint - inp;    // determine error
  
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
