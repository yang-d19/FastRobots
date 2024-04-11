#ifndef CONTROLLER_H
#define CONTROLLER_H

struct ControllerRecord {
    int16_t control;
    int16_t left_control;
    int16_t right_control;
    int16_t left_pwm;
    int16_t right_pwm;
};

void keepDistanceToWall(uint16_t setpoint, uint16_t distance);

ControllerRecord getCtrlRecord();

int16_t pid(int16_t error);

void left_wheel_move(int16_t pwm);

void right_wheel_move(int16_t pwm);

void left_wheel_control(int16_t control);

void right_wheel_control(int16_t control);

int16_t ctrl_pwm_map(int16_t control, int16_t ctrl_min, int16_t ctrl_max, int16_t pwm_min, int16_t pwm_max);

void straight_control(int16_t control);

void stop();


#endif