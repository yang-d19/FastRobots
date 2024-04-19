#include <Arduino.h>
#line 1 "/Users/yangd19/Documents/Code/ECE5160-Code/Lab8/lab8-main/lab8-main.ino"
#include "main.h"

bool prev_auto_running = false;
bool auto_running = false;

uint32_t start_ms, prev_ms, curr_ms, elapsed_ms;



uint16_t distance = 0;
float gyr_z = 0.0;
float yaw = 0.0;

uint16_t goal_distance = 304;
int16_t goal_yaw = 0;

Record records[MAX_RECORDS_LEN];

// set to 1 so that when there is no record in the array, 
// records[record_ptr-1] will not overflow
uint16_t record_ptr = 1;

#line 23 "/Users/yangd19/Documents/Code/ECE5160-Code/Lab8/lab8-main/lab8-main.ino"
void saveCurrRecordToArray();
#line 39 "/Users/yangd19/Documents/Code/ECE5160-Code/Lab8/lab8-main/lab8-main.ino"
uint16_t getCurrRecord(const Record **one_record);
#line 44 "/Users/yangd19/Documents/Code/ECE5160-Code/Lab8/lab8-main/lab8-main.ino"
uint16_t getHistoryRecords(const Record **all_records);
#line 49 "/Users/yangd19/Documents/Code/ECE5160-Code/Lab8/lab8-main/lab8-main.ino"
void setup();
#line 61 "/Users/yangd19/Documents/Code/ECE5160-Code/Lab8/lab8-main/lab8-main.ino"
void loop();
#line 23 "/Users/yangd19/Documents/Code/ECE5160-Code/Lab8/lab8-main/lab8-main.ino"
void saveCurrRecordToArray() {
    ControllerRecord ctrl_record = getCtrlRecord();
    if (record_ptr < MAX_RECORDS_LEN) {
        records[record_ptr].timestamp_ms = elapsed_ms;
        records[record_ptr].distance = distance;
        records[record_ptr].setpoint_distance = goal_distance;
        records[record_ptr].yaw = yaw;
        records[record_ptr].setpoint_yaw = goal_yaw;
        records[record_ptr].left_control = ctrl_record.left_control;
        records[record_ptr].right_control = ctrl_record.right_control;
        records[record_ptr].left_pwm = ctrl_record.left_pwm;
        records[record_ptr].right_pwm = ctrl_record.right_pwm;
        record_ptr++;
    }
}

uint16_t getCurrRecord(const Record **one_record) {
    *one_record = records + (record_ptr - 1);
    return record_ptr - 1;
}

uint16_t getHistoryRecords(const Record **all_records) {
    *all_records = records;
    return record_ptr;
}

void setup() {

    Serial.begin(115200);
    // wait for Serial to start
    while (!Serial) {}

    sensorSetup();
    bleSetup();

    Serial.println("Lab7 main program started");
}

void loop() {
    // Listen for connections
    BLEDevice central = BLE.central();

    // If a central is connected to the peripheral
    if (!central) {
        return;
    }

    Serial.print("Connected to: ");
    Serial.println(central.address());

    // While central is connected
    while (central.connected()) {
        // read and process bluetooth data
        readAndProcessData();

        prev_ms = curr_ms;
        curr_ms = millis();

        // start auto running for the first time
        if (auto_running) {
            if (!prev_auto_running) {
                prev_auto_running = true;
                // reset history records data
                record_ptr = 1;
                // record the start time in millisecond
                start_ms = millis();
                // reset yaw angle
                yaw = 0.0;

                Serial.println("Start auto running");
            }
            else {
                // calculate elapsed time only when auto running
                elapsed_ms = curr_ms - start_ms;
                // read in non-blocking way
                // index 0 is front sensor, index 1 is sideway sensor
                distance = readDistance(0); 
                gyr_z = readGyrZ();
                yaw -= gyr_z * (curr_ms - prev_ms) / 1000.0;

                keepDistanceToWall(goal_distance, distance);
                // keepYaw(goal_yaw, (int16_t)yaw);

                // if (distance <= SAFE_DISTANCE) {
                //     // stop auto running
                //     stop();
                //     // auto_running = false;
                // }
                // else {
                //     stepResponse();
                // }

                // save current record to array
                saveCurrRecordToArray();

                // ====================== For Debugging ======================
                // ControllerRecord ctrl_record = getCtrlRecord();
                // Serial.printf("Time: %d  GyrZ: %d  Yaw: %d  LeftCtrl: %d  RightCtrl: %d\n", 
                //     (int)elapsed_ms, (int)gyr_z, (int)yaw, 
                //     (int)ctrl_record.left_control, (int)ctrl_record.right_control);
                // ====================== Debugging End ======================

                // shutdown after 10 seconds of auto running
                if (elapsed_ms >= MAX_AUTORUN_MS) {
                    stop();
                    auto_running = false;
                    Serial.println("Stop due to time out");
                }
            }

        }
        else {
            if (prev_auto_running) {
                prev_auto_running = false;
                // don't reset record_ptr, or GET_HISTORY_DATA will not work
            }
        }
    }

    // If bluetooth is disconnected
    // busy loop the program until reset button is pressed
    Serial.println("Disconnected, now busy looping...");
    while (1);
}
