#line 1 "/Users/yangd19/Documents/Code/ECE5160-Code/Lab6/lab6-main/sensor.h"
#ifndef _SENSOR_H_
#define _SENSOR_H_

#include <Wire.h>
#include <VL53L1X.h>
#include "ICM_20948.h"

// #define PI 3.14159265358979323

#define SPI_PORT SPI // Your desired SPI port.       Used only when "USE_SPI" is defined
#define CS_PIN 2     // Which pin you connect CS to. Used only when "USE_SPI" is defined

#define WIRE_PORT Wire // Your desired Wire port.      Used when "USE_SPI" is not defined
// The value of the last bit of the I2C address.
// On the SparkFun 9DoF IMU breakout the default is 1, and when the ADR jumper is closed the value becomes 0
#define AD0_VAL 0



// struct Attitude {
//   float roll = 0;
//   float pitch = 0;
//   float yaw = 0;
// };

void sensorSetup(void);

void IMUSensorSetup(void);

void distanceSensorSetup(void);

uint16_t readDistance(uint8_t sensorIndex, bool block=false);

float readGyrZ(bool block=false);

#endif