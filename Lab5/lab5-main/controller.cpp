#include "controller.h"

const int16_t MAX_CONTROL = 500;
const int16_t MIN_CONTROL = -500;

ControllerRecord ctrl_record;

float Kp = 1.5;
float Ki = 0.0;
float Kd = 4;

int16_t integral = 0.0;
int16_t prev_error = 0;


// int prev_integral_ms = 0;
// int curr_integral_ms = 0;

// range of error: -4000 ~ 4000
// range of control: -1000 ~ 1000
// error = distance - setpoint
int16_t pid(int16_t error) {
    int16_t control = Kp * error + Kd * (error - prev_error);
    // integral += error;
    if (control >= MAX_CONTROL) control = MAX_CONTROL;
    if (control <= MIN_CONTROL) control = MIN_CONTROL;
    prev_error = error;
    return control;
}

ControllerRecord getCtrlRecord() {
    return ctrl_record;
}

void keepDistanceToWall(uint16_t setpoint, uint16_t distance) {
    int16_t control = pid(distance - setpoint);
    straight_control(control);
}

// pin 6, 7 have control over right wheels
// pin 11 12 have control over left wheels
void left_wheel_move(int16_t pwm) {
    ctrl_record.left_pwm = pwm;
    if (pwm >= 0) {
        analogWrite(11, 0);
        analogWrite(12, pwm);
    }
    else {
        analogWrite(11, -pwm);
        analogWrite(12, 0);
    }
}

void right_wheel_move(int16_t pwm) {
    ctrl_record.right_pwm = pwm;
    if (pwm >= 0) {
        analogWrite(6, 0);
        analogWrite(7, pwm);
    }
    else {
        analogWrite(6, -pwm);
        analogWrite(7, 0);
    }
}

// dead band for left motors: -30 ~ 50; 55?
// dead band for right motors: -28 ~ 28

int16_t ctrl_pwm_map(int16_t control, int16_t ctrl_min, 
    int16_t ctrl_max, int16_t pwm_min, int16_t pwm_max) {

    int16_t pwm = (control - ctrl_min) * (pwm_max - pwm_min) / (ctrl_max - ctrl_min) + pwm_min;
    return pwm;
}

void left_wheel_control(int16_t control) {
    ctrl_record.left_control = control;
    int16_t left_pwm = 0;
    if (control >= 0) {
        left_pwm = ctrl_pwm_map(control, 0, 1000, 55, 255);
    }
    else {
        left_pwm = ctrl_pwm_map(control, -1000, 0, -255, -30);
    }
    left_wheel_move(left_pwm);
}

void right_wheel_control(int16_t control) {
    ctrl_record.right_control = control;
    int16_t right_pwm = 0;
    if (control >= 0) {
        right_pwm = ctrl_pwm_map(control, 0, 1000, 28, 255);
    }
    else {
        right_pwm = ctrl_pwm_map(control, -1000, 0, -255, -28);
    }
    right_wheel_move(right_pwm);
}

// assume control range from -1000 ~ 1000
void straight_control(int16_t control) {
    // map control signal to actual pwm in order to 
    // eliminate the influence of dead band

    // formula is:  y = (x - x_min) * (y_max - y_min) / (x_max - x_min) + y_min
    ctrl_record.control = control;
    left_wheel_control(control);
    right_wheel_control(control);

    // int left_pwm = 0;
    // int right_pwm = 0;

    // if (control >= 0) {
    //   left_pwm = ctrl_pwm_map(control, 0, 1000, 55, 255);
    //   right_pwm = ctrl_pwm_map(control, 0, 1000, 28, 255);
    //   // left_pwm = int(control * 0.235 + 20.0);
    //   // right_pwm = int(control * 0.215 + 40.0);
    // }
    // else {
    //   left_pwm = ctrl_pwm_map(control, -1000, 0, -255, -30);
    //   right_pwm = ctrl_pwm_map(control, -1000, 0, -255, -28);
    //   // left_pwm = int(control * 0.235 - 20.0);
    //   // right_pwm = int(control * 0.215 - 40.0);
    // }

    // left_wheel_move(left_pwm);
    // right_wheel_move(right_pwm);
}

void stop() {
    left_wheel_move(0);
    right_wheel_move(0);
}