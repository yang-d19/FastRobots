#include "main.h"
#include "ble.h"
#include "controller.h"

BLEService testService(BLE_UUID_TEST_SERVICE);

BLECStringCharacteristic rx_characteristic_string(BLE_UUID_RX_STRING, BLEWrite, MAX_MSG_SIZE);

BLEFloatCharacteristic tx_characteristic_float(BLE_UUID_TX_FLOAT, BLERead | BLENotify);
BLECStringCharacteristic tx_characteristic_string(BLE_UUID_TX_STRING, BLERead | BLENotify, MAX_MSG_SIZE);

// RX
RobotCommand robot_cmd(":|");

// TX
EString tx_estring_value;
float tx_float_value = 0.0;

// control the running state of the robot
extern bool auto_running;
extern uint16_t goal_distance;

void bleSetup() {
    BLE.begin();
    // Set advertised local name and service
    BLE.setDeviceName("Artemis BLE");
    BLE.setLocalName("Artemis BLE");
    BLE.setAdvertisedService(testService);

    // Add BLE characteristics
    testService.addCharacteristic(tx_characteristic_float);
    testService.addCharacteristic(tx_characteristic_string);
    testService.addCharacteristic(rx_characteristic_string);

    // Add BLE service
    BLE.addService(testService);

    // Initial values for characteristics
    // Set initial values to prevent errors when reading for the first time on central devices
    tx_characteristic_float.writeValue(0.0);

    // Output MAC Address
    Serial.print("Advertising BLE with MAC: ");
    Serial.println(BLE.address());

    BLE.advertise();
}

void readAndProcessData() {
    if (rx_characteristic_string.written())  {
        handleCommand();
    }
}

void putARecordToTxEstring(const Record *record, uint16_t index) {
    // Serial.println("loc0");
    tx_estring_value.clear();
    tx_estring_value.append((int)index);
    // Serial.println("loc1");
    // Serial.println(tx_estring_value.c_str());
    tx_estring_value.append(":");
    tx_estring_value.append((int)record->timestamp_ms);
    // Serial.println("loc2");
    // Serial.println(tx_estring_value.c_str());
    tx_estring_value.append("|");
    tx_estring_value.append((int)record->distance);
    // Serial.println("loc3");
    // Serial.println(tx_estring_value.c_str());
    tx_estring_value.append("|");
    tx_estring_value.append((int)record->control);
    // Serial.println("loc4");
    // Serial.println(tx_estring_value.c_str());
    tx_estring_value.append("|");
    tx_estring_value.append((int)record->left_control);
    // Serial.println("loc5");
    // Serial.println(tx_estring_value.c_str());
    tx_estring_value.append("|");
    tx_estring_value.append((int)record->right_control);
    // Serial.println("loc6");
    // Serial.println(tx_estring_value.c_str());
    tx_estring_value.append("|");
    tx_estring_value.append((int)record->left_pwm);
    // Serial.println("loc7");
    // Serial.println(tx_estring_value.c_str());
    tx_estring_value.append("|");
    tx_estring_value.append((int)record->right_pwm);
}

void handleCommand() {   
    // Set the command string from the characteristic value
    robot_cmd.set_cmd_string(rx_characteristic_string.value(),
                             rx_characteristic_string.valueLength());

    bool success;
    int cmd_type = -1;

    // Get robot command type (an integer)
    /* NOTE: THIS SHOULD ALWAYS BE CALLED BEFORE get_next_value()
     * since it uses strtok internally (refer RobotCommand.h and 
     * https://www.cplusplus.com/reference/cstring/strtok/)
     */
    success = robot_cmd.get_command_type(cmd_type);

    // Check if the last tokenization was successful and return if failed
    if (!success) {
        return;
    }

    // Handle the command type accordingly
    switch (cmd_type) {
        /*
         * Write "PONG" on the GATT characteristic BLE_UUID_TX_STRING
         */
        case PING: {
            tx_estring_value.clear();
            tx_estring_value.append("PONG");
            tx_characteristic_string.writeValue(tx_estring_value.c_str());

            Serial.print("Sent back: ");
            Serial.println(tx_estring_value.c_str());

            break;
        }

        /*
         * Add a prefix and postfix to the string value extracted from the command string
         */
        case ECHO: {
            char char_arr[MAX_MSG_SIZE];
            // Extract the next value from the command string as a character array
            success = robot_cmd.get_next_value(char_arr);
            if (!success)
                return;

            tx_estring_value.clear();
            tx_estring_value.append("Robot echos ");
            tx_estring_value.append(char_arr);
            tx_estring_value.append(" ^_^");
            tx_characteristic_string.writeValue(tx_estring_value.c_str());

            Serial.print("Sent back: ");
            Serial.println(tx_estring_value.c_str());
            
            break;
        }
        
        case STOP: {
            // stop the robot and stop auto running
            stop();
            auto_running = false;

            tx_estring_value.clear();
            tx_estring_value.append("Succeed: brake and stop auto running");
            tx_characteristic_string.writeValue(tx_estring_value.c_str());

            break;
        }
        
        case START_AUTO: {
            auto_running = true;

            tx_estring_value.clear();
            tx_estring_value.append("Succeed: start auto running");
            tx_characteristic_string.writeValue(tx_estring_value.c_str());

            break;
        }


        case STOP_AUTO: {
            auto_running = false;

            tx_estring_value.clear();
            tx_estring_value.append("Succeed: stop auto running");
            tx_characteristic_string.writeValue(tx_estring_value.c_str());

            break;
        }
        
        case SET_TWO_PWM: {

            tx_estring_value.clear();

            if (auto_running) {
                tx_estring_value.append("Failed: already auto running");
                tx_characteristic_string.writeValue(tx_estring_value.c_str());
                break;
            }

            int left_pwm, right_pwm;

            // Extract the next value from the command string as an integer
            success = robot_cmd.get_next_value(left_pwm);
            if (!success) {
                tx_estring_value.append("Failed: unable to get first int");
                tx_characteristic_string.writeValue(tx_estring_value.c_str());
                break;
            }
            // Extract the next value from the command string as an integer
            success = robot_cmd.get_next_value(right_pwm);
            if (!success) {
                tx_estring_value.append("Failed: unable to get second int");
                tx_characteristic_string.writeValue(tx_estring_value.c_str());
                break;
            }
            
            // Set the PWM values
            if (left_pwm < -255 || left_pwm > 255
                || right_pwm < -255 || right_pwm > 255) {
                tx_estring_value.append("Failed: pwm values out of range");
                tx_characteristic_string.writeValue(tx_estring_value.c_str());
                break;
            }

            tx_estring_value.append("Succeed: get two pwms ");
            tx_estring_value.append(left_pwm);
            tx_estring_value.append(", ");
            tx_estring_value.append(right_pwm);
            tx_characteristic_string.writeValue(tx_estring_value.c_str());

            left_wheel_move(left_pwm);
            right_wheel_move(right_pwm);

            break;
        }
        
        case SET_TWO_CTRL: {

            tx_estring_value.clear();

            if (auto_running) {
                tx_estring_value.append("Failed: already auto running");
                tx_characteristic_string.writeValue(tx_estring_value.c_str());
                break;
            }

            int left_ctrl, right_ctrl;

            // Extract the next value from the command string as an integer
            success = robot_cmd.get_next_value(left_ctrl);
            if (!success) {
                tx_estring_value.append("Failed: unable to get first int");
                tx_characteristic_string.writeValue(tx_estring_value.c_str());
                break;
            }
            // Extract the next value from the command string as an integer
            success = robot_cmd.get_next_value(right_ctrl);
            if (!success) {
                tx_estring_value.append("Failed: unable to get second int");
                tx_characteristic_string.writeValue(tx_estring_value.c_str());
                break;
            }
            
            // Set the PWM values
            if (left_ctrl < -1000 || left_ctrl > 1000
                || right_ctrl < -1000 || right_ctrl > 1000) {
                tx_estring_value.append("Failed: ctrl values out of range");
                tx_characteristic_string.writeValue(tx_estring_value.c_str());
                break;
            }

            tx_estring_value.append("Succeed: get two ctrls ");
            tx_estring_value.append(left_ctrl);
            tx_estring_value.append(", ");
            tx_estring_value.append(right_ctrl);
            tx_characteristic_string.writeValue(tx_estring_value.c_str());

            left_wheel_control(left_ctrl);
            right_wheel_control(right_ctrl);

            break;
        }
        
        /* 
         * Return current time in milliseconds
         */
        case GET_CURR_DATA: {
            
            const Record *record;
            uint16_t idx = getCurrRecord(&record);

            putARecordToTxEstring(record, idx);

            tx_characteristic_string.writeValue(tx_estring_value.c_str());

            break;
        }

        case GET_HISTORY_DATA: {
            // forbidden transfering history data when robot is auto running
            if (auto_running) {
                tx_estring_value.append("Failed: auto running, try later");
                tx_characteristic_string.writeValue(tx_estring_value.c_str());
                break;
            }

            const Record *records;
            uint16_t records_size = getHistoryRecords(&records);

            tx_estring_value.clear();
            tx_estring_value.append("Size of records array: ");
            tx_estring_value.append(records_size);
            tx_characteristic_string.writeValue(tx_estring_value.c_str());

            for (int i = 0; i < records_size; i++) {
                putARecordToTxEstring(records + i, i);
                tx_characteristic_string.writeValue(tx_estring_value.c_str());
            }
            break;
        }

        case SET_GOAL_DISTANCE: {
            tx_estring_value.clear();
            int num = 0;
            // Extract the next value from the command string as an integer
            success = robot_cmd.get_next_value(num);
            if (!success) {
                tx_estring_value.append("Failed: unable to get goal distance");
                tx_characteristic_string.writeValue(tx_estring_value.c_str());
                break;
            }
            if (num < 0 || num > 1000) {
                tx_estring_value.append("Failed: goal distance out of range");
                tx_characteristic_string.writeValue(tx_estring_value.c_str());
                break;
            }
            goal_distance = num;
        }

        /* 
         * The default case may not capture all types of invalid commands.
         * It is safer to validate the command string on the central device (in python)
         * before writing to the characteristic.
         */
        default: {
            Serial.print("Invalid Command Type: ");
            Serial.println(cmd_type);
            break;
        }
    }
}
