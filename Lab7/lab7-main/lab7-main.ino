#include <Wire.h>
#include <VL53L1X.h>

#include "BLECStringCharacteristic.h"
#include "EString.h"
#include "RobotCommand.h"
#include <ArduinoBLE.h>



// The number of sensors in your system.
const uint8_t sensorCount = 2;

// The Arduino pin connected to the XSHUT pin of each sensor.
const uint8_t xshutPins[sensorCount] = { 15, 16 };

VL53L1X sensors[sensorCount];

unsigned long startMillis, currMillis, elapsedMillis;

void setup() {
  // wait for Serial to start
  while (!Serial) {}

  Serial.begin(115200);
  Serial.println("Lab5 main code");

  Wire.begin();
  Wire.setClock(400000); // use 400 kHz I2C

  // Disable/reset all sensors by driving their XSHUT pins low.
  for (uint8_t i = 0; i < sensorCount; i++)
  {
    pinMode(xshutPins[i], OUTPUT);
    digitalWrite(xshutPins[i], LOW);
  }

  // Enable, initialize, and start each sensor, one by one.
  for (uint8_t i = 0; i < sensorCount; i++)
  {
    // Stop driving this sensor's XSHUT low. This should allow the carrier
    // board to pull it high. (We do NOT want to drive XSHUT high since it is
    // not level shifted.) Then wait a bit for the sensor to start up.
    pinMode(xshutPins[i], INPUT);
    delay(10);

    sensors[i].setTimeout(500);
    if (!sensors[i].init())
    {
      Serial.print("Failed to detect and initialize sensor ");
      Serial.println(i);
      while (1);
    }

    sensors[i].setDistanceMode(VL53L1X::Short);
    sensors[i].setMeasurementTimingBudget(20000);

    // Each sensor must have its address changed to a unique value other than
    // the default of 0x29 (except for the last one, which could be left at
    // the default). To make it simple, we'll just count up from 0x2A.
    sensors[i].setAddress(0x2A + i);

    sensors[i].startContinuous(20);
  }

  // record the start time in millisecond
  startMillis = millis();
}




void loop() {
  currMillis = millis();
  elapsedMillis = currMillis - startMillis;

  distance = sensors[0].read(false); // non-blocking way
  control = pid(distance - setpoint);

  // comment this out in real control mode to accelerate the loop
  Serial.printf("%d (ms): Distance: %d   Control: %d\n", elapsed_ms, distance, control);

  // move straightly as pid controller indicates
  straight_move(control);

  if (elapsed_ms >= 9000) {
    stop();
    while (1);
  }

  // // simple test
  // if (elapsed_ms >= 8000) {
  //   stop();
  //   while (1);
  // }
  // else if (elapsed_ms >= 3500) {
  //   straight_move(-1);

  //   // left_wheel_move(-30);
  //   // right_wheel_move(-28);
  // }
  // else {
  //   straight_move(1);

  //   // left_wheel_move(50);
  //   // right_wheel_move(28);
  // }

  delay(10);
}
