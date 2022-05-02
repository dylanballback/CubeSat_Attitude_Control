#pragma once

#include "defines.h"

#include <WebSockets2_Generic.h> 
using namespace websockets2_generic;
WebsocketsClient client;

#include <SAMD21turboPWM.h>
#include <Wire.h>        // IIC communication library
#include <Arduino_LSM6DS3.h>

TurboPWM pwm;

#include "motor.h"
#include "websocket.h"
#include "imu.h"
#include "thermister.h"
#include "photoresister.h"
#include "mpu.h"
#include "PID.h"
#include "encoder.h"
