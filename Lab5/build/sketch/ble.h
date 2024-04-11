#line 1 "/Users/yangd19/Documents/Code/ECE5160-Code/Lab5/lab5-main/ble.h"
#ifndef BLE_H
#define BLE_H

#include <ArduinoBLE.h>
#include "BLECStringCharacteristic.h"
#include "RobotCommand.h"
#include "EString.h"

//////////// BLE UUIDs ////////////
#define BLE_UUID_TEST_SERVICE "e1e57366-5298-47b4-9844-5af935d8a194"

#define BLE_UUID_RX_STRING "9750f60b-9c9c-4158-b620-02ec9521cd99"

#define BLE_UUID_TX_FLOAT "27616294-3063-4ecc-b60b-3470ddef2938"
#define BLE_UUID_TX_STRING "f235a225-6735-4d73-94cb-ee5dfce9ba83"
//////////// BLE UUIDs ////////////

enum CommandTypes {
    PING,
    ECHO,
    STOP,
    START_AUTO,
    STOP_AUTO,
    SET_TWO_PWM,
    SET_TWO_CTRL,
    GET_CURR_DATA,
    GET_HISTORY_DATA,
    SET_GOAL_DISTANCE
};

void bleSetup();
void handleCommand();

void readAndProcessData();

#endif