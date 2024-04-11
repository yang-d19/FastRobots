#line 1 "/Users/yangd19/Documents/Code/ECE5160-Code/Lab5/lab5-main/sensor.cpp"
#include "sensor.h"

// The number of sensors in your system.
const uint8_t sensorCount = 2;

// The Arduino pin connected to the XSHUT pin of each sensor.
const uint8_t xshutPins[sensorCount] = { 15, 16 };

VL53L1X sensors[sensorCount];

ICM_20948_I2C myICM; // Otherwise create an ICM_20948_I2C object

//--------------------------------Newly added imu sensor--------------------------------
Attitude att;

const int buffer_size = 2000;
int end_pt = 0;
int time_stamps[buffer_size];
Attitude atts[buffer_size];

//--------------------------------Newly added imu sensor--------------------------------

void sensorSetup() {
    Wire.begin();
    Wire.setClock(400000); // use 400 kHz I2C

    distanceSensorSetup();

    // IMUSensorSetup();
}

void IMUSensorSetup() {
    bool initialized = false;
    while (!initialized) {
        myICM.begin(WIRE_PORT, AD0_VAL);

        Serial.print("Initialization of the sensor returned: ");
        Serial.println(myICM.statusString());
        if (myICM.status != ICM_20948_Stat_Ok) {
            Serial.println("Trying again...");
            delay(500);
        }
        else {
            initialized = true;
        }
    }
}

void distanceSensorSetup(void) {

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
}

uint16_t readDistance(uint8_t sensorIndex, bool block) {
    return sensors[sensorIndex].read(block);
}