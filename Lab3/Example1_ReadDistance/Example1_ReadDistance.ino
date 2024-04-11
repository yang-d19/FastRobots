/*
  Reading distance from the laser based VL53L1X
  By: Nathan Seidle
  SparkFun Electronics
  Date: April 4th, 2018
  License: This code is public domain but you buy me a beer if you use this and we meet someday (Beerware license).

  SparkFun labored with love to create this code. Feel like supporting open source hardware?
  Buy a board from SparkFun! https://www.sparkfun.com/products/14667

  This example prints the distance to an object.

  Are you getting weird readings? Be sure the vacuum tape has been removed from the sensor.
*/

#include <Wire.h>
#include "SparkFun_VL53L1X.h" //Click here to get the library: http://librarymanager/All#SparkFun_VL53L1X

//Optional interrupt and shutdown pins.
#define SHUTDOWN_PIN 2
#define INTERRUPT_PIN 3

SFEVL53L1X distanceSensor1;
SFEVL53L1X distanceSensor2;

//Uncomment the following line to use the optional shutdown and interrupt pins.
//SFEVL53L1X distanceSensor(Wire, SHUTDOWN_PIN, INTERRUPT_PIN);

void setup(void)
{
  Wire.begin();

  Serial.begin(115200);
  Serial.println("VL53L1X Qwiic Test");

  if (distanceSensor1.begin() != 0) //Begin returns 0 on a good init
  {
    Serial.println("Sensor1 failed to begin. Please check wiring. Freezing...");
    while (1)
      ;
  }

  Serial.println("Sensor1 online!");

  Serial.printf("old sensor1 i2c addr: %d\n", distanceSensor1.getI2CAddress()); // 82

  distanceSensor1.setI2CAddress(80);
  Serial.printf("sensor1 i2c addr: %d\n", distanceSensor1.getI2CAddress());

  if (distanceSensor2.begin() != 0) //Begin returns 0 on a good init
  {
    Serial.println("Sensor2 failed to begin. Please check wiring. Freezing...");
    while (1)
      ;
  }

  Serial.println("Sensor2 online!");

  Serial.printf("sensor2 i2c addr: %d\n", distanceSensor2.getI2CAddress());

  
  // distanceSensor1.setDistanceModeShort();

  // //Get the distance mode, returns 1 for short and 2 for long
  // int mode = distanceSensor1.getDistanceMode();
  // Serial.printf("Sensor Mode: %d\n", mode);
}

void loop(void)
{
  // distanceSensor.startRanging(); //Write configuration bytes to initiate measurement
  // while (!distanceSensor.checkForDataReady())
  // {
  //   delay(1);
  // }
  // int distance = distanceSensor.getDistance(); //Get the result of the measurement from the sensor
  // distanceSensor.clearInterrupt();
  // distanceSensor.stopRanging();

  // Serial.print("Distance(mm): ");
  // Serial.print(distance);

  // float distanceInches = distance * 0.0393701;
  // float distanceFeet = distanceInches / 12.0;

  // Serial.print("\tDistance(ft): ");
  // Serial.print(distanceFeet, 2);

  // Serial.println();
}
