#ifndef _MAIN_H_
#define _MAIN_H_

#include "sensor.h"
#include "ble.h"
#include "controller.h"

// by calculation, max len is 20000
// fastest frequency is 1000ms/2ms = 500Hz
// so max record time is 10000/500Hz = 20s
#define MAX_RECORDS_LEN 3000

// max autorunning time set to 10s
// to protect the robot when controller is down
#define MAX_AUTORUN_MS 12000

struct Record {
    uint32_t timestamp_ms;
    uint16_t distance;
    uint16_t setpoint_distance;
    int16_t yaw;
    int16_t setpoint_yaw;
    int16_t left_control;
    int16_t right_control;
    int16_t left_pwm;
    int16_t right_pwm;
};

/*
 * get current record
 */
uint16_t getCurrRecord(const Record **one_record);

/*
 * Get all history records, return data array from parameter pointer
 * return length of data array from return value
*/
uint16_t getHistoryRecords(const Record **all_records);

#endif