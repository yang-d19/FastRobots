#line 1 "/Users/yangd19/Documents/Code/ECE5160-Code/Lab7/lab7-main/sensor.cpp"
#include "sensor.h"

// The number of sensors in your system.
const uint8_t sensorCount = 2;

// The Arduino pin connected to the XSHUT pin of each sensor.
const uint8_t xshutPins[sensorCount] = { 15, 16 };

VL53L1X sensors[sensorCount];

ICM_20948_I2C myICM; // Otherwise create an ICM_20948_I2C object

float gyr_z_ = 0.0;

//--------------------------------Newly added imu sensor--------------------------------
// Attitude att;

// Attitude atts[buffer_size];

// uint16_t curr_ms_ = 0;
// uint16_t prev_ms_ = 0;

//--------------------------------Newly added imu sensor--------------------------------

void sensorSetup() {
    Wire.begin();
    Wire.setClock(400000); // use 400 kHz I2C

    distanceSensorSetup();
    IMUSensorSetup();
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
    Serial.println("IMU Sensor initialized");
}

void distanceSensorSetup(void) {
    // Disable/reset all sensors by driving their XSHUT pins low.
    for (uint8_t i = 0; i < sensorCount; i++) {
        pinMode(xshutPins[i], OUTPUT);
        digitalWrite(xshutPins[i], LOW);
    }

    // Enable, initialize, and start each sensor, one by one.
    for (uint8_t i = 0; i < sensorCount; i++) {
        // Stop driving this sensor's XSHUT low. This should allow the carrier
        // board to pull it high. (We do NOT want to drive XSHUT high since it is
        // not level shifted.) Then wait a bit for the sensor to start up.
        pinMode(xshutPins[i], INPUT);
        delay(10);

        sensors[i].setTimeout(500);
        if (!sensors[i].init()) {
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
    Serial.println("Distance Sensor initialized");
}

uint16_t readDistance(uint8_t sensorIndex, bool block) {
    return sensors[sensorIndex].read(block);
}

float readGyrZ(bool block) {
    // if blocking is true, wait for new gyroscope data to be ready
    if (block == true) {
        while (!myICM.dataReady());
    }
    // if new data is available, update it
    // else just return previous data
    if (myICM.dataReady()) {
        myICM.getAGMT();
        gyr_z_ = myICM.gyrZ();
    }
    return gyr_z_;
}

// Below here are some helper functions to print the data nicely!

void printPaddedInt16b(int16_t val)
{
    if (val > 0) {
        Serial.print(" ");
        if (val < 10000) {
          Serial.print("0");
        }
        if (val < 1000) {
          Serial.print("0");
        }
        if (val < 100) {
          Serial.print("0");
        }
        if (val < 10) {
          Serial.print("0");
        }
    }
    else
    {
        Serial.print("-");
        if (abs(val) < 10000) {
          Serial.print("0");
        }
        if (abs(val) < 1000) {
          Serial.print("0");
        }
        if (abs(val) < 100) {
          Serial.print("0");
        }
        if (abs(val) < 10) {
          Serial.print("0");
        }
    }
    Serial.print(abs(val));
}

void printRawAGMT(ICM_20948_AGMT_t agmt)
{
    Serial.print("RAW. Acc [ ");
    printPaddedInt16b(agmt.acc.axes.x);
    Serial.print(", ");
    printPaddedInt16b(agmt.acc.axes.y);
    Serial.print(", ");
    printPaddedInt16b(agmt.acc.axes.z);
    Serial.print(" ], Gyr [ ");
    printPaddedInt16b(agmt.gyr.axes.x);
    Serial.print(", ");
    printPaddedInt16b(agmt.gyr.axes.y);
    Serial.print(", ");
    printPaddedInt16b(agmt.gyr.axes.z);
    Serial.print(" ], Mag [ ");
    printPaddedInt16b(agmt.mag.axes.x);
    Serial.print(", ");
    printPaddedInt16b(agmt.mag.axes.y);
    Serial.print(", ");
    printPaddedInt16b(agmt.mag.axes.z);
    Serial.print(" ], Tmp [ ");
    printPaddedInt16b(agmt.tmp.val);
    Serial.print(" ]");
    Serial.println();
}

void printFormattedFloat(float val, uint8_t leading, uint8_t decimals)
{
    float aval = abs(val);
    if (val < 0) {
        Serial.print("-");
    }
    else {
        Serial.print(" ");
    }
    for (uint8_t indi = 0; indi < leading; indi++) {
        uint32_t tenpow = 0;
        if (indi < (leading - 1)) {
            tenpow = 1;
        }
        for (uint8_t c = 0; c < (leading - 1 - indi); c++) {
            tenpow *= 10; 
        }
        if (aval < tenpow) {
            Serial.print("0");
        }
        else {
            break;
        }
    }
    if (val < 0) {
        Serial.print(-val, decimals);
    }
    else {
        Serial.print(val, decimals);
    }
}

// void convertAccel2Attitude(ICM_20948_I2C *sensor, Attitude* att) {
//     float pitch_rad = atan2(sensor->accX(), -sensor->accZ());
//     float roll_rad = atan2(sensor->accY(), -sensor->accZ());
//     att->pitch = pitch_rad * 180.0 / PI;
//     att->roll = roll_rad * 180.0 / PI;
// }

// void convertGyro2Attitude(ICM_20948_I2C *sensor, Attitude* att) {
//   float gyr_x = sensor->gyrX();
//   float roll_rad = atan2(sensor->accY(), -sensor->accZ());
//   att->pitch = pitch_rad * 180.0 / M_PI;
//   att->roll = roll_rad * 180.0 / M_PI;
// }

// void printAttitude(Attitude *att) {
//     Serial.print("Pitch [ ");
//     printFormattedFloat(att->pitch, 3, 2);
//     Serial.print(" ], Roll [ ");
//     printFormattedFloat(att->roll, 3, 2);
//     Serial.print(" ], Yaw [ ");
//     printFormattedFloat(att->yaw, 3, 2);
//     Serial.print(" ]");
//     Serial.println();
// }

void printScaledAGMT(ICM_20948_I2C *sensor) {
    Serial.print("Scaled. Acc (mg) [ ");
    printFormattedFloat(sensor->accX(), 5, 2);
    Serial.print(", ");
    printFormattedFloat(sensor->accY(), 5, 2);
    Serial.print(", ");
    printFormattedFloat(sensor->accZ(), 5, 2);
    Serial.print(" ], Gyr (DPS) [ ");
    printFormattedFloat(sensor->gyrX(), 5, 2);
    Serial.print(", ");
    printFormattedFloat(sensor->gyrY(), 5, 2);
    Serial.print(", ");
    printFormattedFloat(sensor->gyrZ(), 5, 2);
    Serial.print(" ], Mag (uT) [ ");
    printFormattedFloat(sensor->magX(), 5, 2);
    Serial.print(", ");
    printFormattedFloat(sensor->magY(), 5, 2);
    Serial.print(", ");
    printFormattedFloat(sensor->magZ(), 5, 2);
    Serial.print(" ], Tmp (C) [ ");
    printFormattedFloat(sensor->temp(), 5, 2);
    Serial.print(" ]");
    Serial.println();
}