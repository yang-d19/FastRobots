#include <Arduino.h>
#line 1 "/Users/yangd19/Documents/Code/ECE5160-Code/Lab5/lab5-main/lab5-main.ino"
#include "main.h"

bool prev_auto_running = false;
bool auto_running = false;

uint32_t start_ms, curr_ms, elapsed_ms;

uint16_t distance = 0;

uint16_t goal_distance = 304;

Record records[MAX_RECORDS_LEN];

// set to 1 so that when there is no record in the array, 
// records[record_ptr-1] will not overflow
uint16_t record_ptr = 1;

#line 18 "/Users/yangd19/Documents/Code/ECE5160-Code/Lab5/lab5-main/lab5-main.ino"
void saveCurrRecordToArray();
#line 32 "/Users/yangd19/Documents/Code/ECE5160-Code/Lab5/lab5-main/lab5-main.ino"
uint16_t getCurrRecord(const Record **one_record);
#line 37 "/Users/yangd19/Documents/Code/ECE5160-Code/Lab5/lab5-main/lab5-main.ino"
uint16_t getHistoryRecords(const Record **all_records);
#line 42 "/Users/yangd19/Documents/Code/ECE5160-Code/Lab5/lab5-main/lab5-main.ino"
void setup();
#line 54 "/Users/yangd19/Documents/Code/ECE5160-Code/Lab5/lab5-main/lab5-main.ino"
void loop();
#line 18 "/Users/yangd19/Documents/Code/ECE5160-Code/Lab5/lab5-main/lab5-main.ino"
void saveCurrRecordToArray() {
    ControllerRecord ctrl_record = getCtrlRecord();
    if (record_ptr < MAX_RECORDS_LEN) {
        records[record_ptr].timestamp_ms = elapsed_ms;
        records[record_ptr].distance = distance;
        records[record_ptr].control = ctrl_record.control;
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
    // wait for Serial to start
    while (!Serial) {}

    Serial.begin(115200);

    bleSetup();
    distanceSensorSetup();

    Serial.println("Lab5 main program started");
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

        // start auto running for the first time
        if (auto_running) {
            if (!prev_auto_running) {
                prev_auto_running = true;
                // reset history records data
                record_ptr = 1;
                // record the start time in millisecond
                start_ms = millis();
            }
            curr_ms = millis();
            elapsed_ms = curr_ms - start_ms;

            // read in non-blocking way
            distance = readDistance(0); 
            // save current record to array
            saveCurrRecordToArray();

            keepDistanceToWall(goal_distance, distance);

            // shutdown after 10 seconds of auto running
            if (elapsed_ms >= MAX_AUTORUN_MS) {
                stop();
                auto_running = false;
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

// void write_data() {
//     currentMillis = millis();
//     if (currentMillis - previousMillis > 0) {

//         // tx_float_value = tx_float_value + 0.5;
//         // tx_characteristic_float.writeValue(tx_float_value);

//         // if (tx_float_value > 10000) {
//         //     tx_float_value = 0;
//         // }

//         // 1. Send time millisec as fast as possible
//         tx_estring_value.clear();
//         tx_estring_value.append("T: ");
//         tx_estring_value.append((const int)currentMillis);
//         tx_characteristic_string.writeValue(tx_estring_value.c_str());

//         // 2. Don't directly send, save to array instead
//         // if (end_pt < buffer_size) {
//         //   time_stamps[end_pt] = currentMillis;
//         //   temperatures[end_pt] = getTempDegF();
//         //   end_pt++;
//         // }

//         previousMillis = currentMillis;
//     }
// }

// void readAndProcessData() {
//     Query if the characteristic value has been written by another BLE device
//     if (rx_characteristic_string.written()) {
//         handleCommand();
//     }
// }



/*
// move straightly as pid controller indicates
// straight_move(control);

// if (elapsed_ms >= 9000) {
//   stop();
//   while (1);
// }

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
*/
