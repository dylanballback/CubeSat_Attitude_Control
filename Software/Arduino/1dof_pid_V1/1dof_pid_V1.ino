#include <MPU6050.h>
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

MPU6050 mpu;
// Timers
unsigned long timer = 0;
float timeStep = 0.01;

// Roll values
float roll = 0;


//---------------------------------------- PID Vars START -------------------------------------------------------
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

  

  Serial.begin(115200);
  // Initialize MPU6050
  while(!mpu.begin(MPU6050_SCALE_2000DPS, MPU6050_RANGE_2G))
  {
    Serial.println("Could not find a valid MPU6050 sensor, check wiring!");
    delay(500);
  }
  
  // Calibrate gyroscope. The calibration must be at rest.
  // If you don't want calibrate, comment this line.
  mpu.calibrateGyro();

  // Set threshold sensivty. Default 3.
  // If you don't want use threshold, comment this line or set 0.
  mpu.setThreshold(3);
}
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% Setup Function END %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% Loop Function START %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
void loop() {
  timer = millis();
  Vector norm = mpu.readNormalizeGyro();   // Read normalized values
  roll = roll + norm.XAxis * timeStep;     // Calculate Roll

  
  double angle = roll;
  Input = angle;
  
  //Calculate the PID output
  Output = computePID(Input);
  //Serial.print("Output:");
  //Serial.print(Output);
  //Serial.print("   ");

  
  
  
    //Switches the direction of the wheel based on sign of PID output
  if (Output > 1){
    dir = 1; //CW
    //This is just fliping the PWM value from 0 to 390 ---> to 390 to 0
    //Whereas 0 = stopped and 390 = full speed
    pwm = map(Output, 0, outMax, 400, 0);
    //Serial.print("dir: ");
    //Serial.print("CW");
    //Serial.print("    ");
  }
  else if (Output < 0){
    dir = 0; //CCW
    //This is just fliping the PWM value from -390 to 0 ---> to 390 to 0
    //Whereas 0 = stopped and 390 = full speed
    pwm = map(Output, 0, outMin, 400, 0);
    //Serial.print("dir: ");
    //Serial.print("CCW");
    //Serial.print("    ");
  }


  if (angle > -bounds && angle < bounds){
    pwm = 400;
  }
  Serial.println(angle);
  /*
  Serial.print("angle:");
  Serial.print(angle);
  Serial.print("   "); 
  Serial.print("Output:");
  Serial.print(Output);
  Serial.print("   "); 
  Serial.print("pwm:");
  Serial.println(pwm);
  */
  //This is the comand to drive the motor
  driveMotor(pwm, dir);
  

}//end of loop void
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% Loop Function END %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%




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



//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ Compute PID Function END $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
