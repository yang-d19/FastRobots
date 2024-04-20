#ifndef CONTROLLER_H
#define CONTROLLER_H

// in order to use std::pair
#include <utility>

struct ControllerRecord {
    int16_t left_control;
    int16_t right_control;
    int16_t left_pwm;
    int16_t right_pwm;
};

struct PIDParam {
    float Kp;
    float Ki;
    float Kd;
};

struct PIDVariable {
    int16_t prev_error;
    int16_t integral;
};

void resetPIDController(void);

void keepDistanceToWall(uint16_t setpoint, uint16_t distance);

void keepYaw(int16_t setpoint, int16_t yaw);

void fullPowerForward();

void forwardKeepYaw(int16_t setpoint_yaw, int16_t yaw, int16_t straight_control);

ControllerRecord getCtrlRecord();

std::pair<int16_t, int16_t> position_pid(int16_t error);

std::pair<int16_t, int16_t> angle_pid(int16_t error);

void left_wheel_move(int16_t pwm);

void right_wheel_move(int16_t pwm);

void left_wheel_control(int16_t control);

void right_wheel_control(int16_t control);

int16_t ctrl_pwm_map(int16_t control, int16_t ctrl_min, int16_t ctrl_max, int16_t pwm_min, int16_t pwm_max);

void straight_control(int16_t control);

void stop();


#endif