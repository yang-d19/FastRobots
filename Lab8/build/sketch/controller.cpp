#line 1 "/Users/yangd19/Documents/Code/ECE5160-Code/Lab8/lab8-main/controller.cpp"
#include "controller.h"

const int16_t MAX_CONTROL = 900;
const int16_t MIN_CONTROL = -900;

ControllerRecord ctrl_record;

// PID parameters for position control
PIDParam pos_param = {
    .Kp = 1.5,
    .Ki = 0,
    .Kd = 10
};
// PID parameters for angle control
PIDParam angle_param = {
    .Kp = 50,
    .Ki = 5,
    .Kd = 15
};

int16_t integral = 0.0;

PIDVariable pos_var, angle_var;

void resetPIDController(void) {
    pos_var.prev_error = 0.0;
    pos_var.integral = 0.0;
    angle_var.prev_error = 0.0;
    angle_var.integral = 0.0;
}


// int prev_integral_ms = 0;
// int curr_integral_ms = 0;

// range of error: -4000 ~ 4000
// range of control: -1000 ~ 1000
// error = distance - setpoint
std::pair<int16_t, int16_t> position_pid(int16_t error) {
    int16_t control = pos_param.Kp * error + pos_param.Kd * (error - pos_var.prev_error);
    // integral += error;
    if (control >= MAX_CONTROL) control = MAX_CONTROL;
    if (control <= MIN_CONTROL) control = MIN_CONTROL;
    pos_var.prev_error = error;
    return std::make_pair(control, control);
}

// error = angle - setpoint
std::pair<int16_t, int16_t> angle_pid(int16_t error) {

    // 这边可能有 bug，旋转超过 180 度之后，yaw 的表示会有问题吗
    // 应该不影响，yaw 是连续的，只是多个不同的 yaw 实际上表示相同的角度朝向

    angle_var.integral += angle_param.Ki * error;

    int16_t control = angle_param.Kp * error 
        + angle_param.Kd * (error - angle_var.prev_error)
        + angle_var.integral;
    
    if (control >= MAX_CONTROL) control = MAX_CONTROL;
    if (control <= MIN_CONTROL) control = MIN_CONTROL;

    angle_var.prev_error = error;
    return std::make_pair(-control, control);
}

ControllerRecord getCtrlRecord() {
    return ctrl_record;
}

void keepDistanceToWall(uint16_t setpoint_dist, uint16_t distance) {
    std::pair<int16_t, int16_t> two_controlls = position_pid(distance - setpoint_dist);

    ctrl_record.left_control = two_controlls.first;
    ctrl_record.right_control = two_controlls.second;
    left_wheel_control(two_controlls.first);
    right_wheel_control(two_controlls.second);
}

void keepYaw(int16_t setpoint_yaw, int16_t yaw) {
    std::pair<int16_t, int16_t> two_controlls = angle_pid(yaw - setpoint_yaw);

    ctrl_record.left_control = two_controlls.first;
    ctrl_record.right_control = two_controlls.second;
    left_wheel_control(two_controlls.first);
    right_wheel_control(two_controlls.second);
}

void forwardKeepYaw(int16_t setpoint_yaw, int16_t yaw, int16_t straight_control) {
    std::pair<int16_t, int16_t> two_controlls = angle_pid(yaw - setpoint_yaw);

    // if turning component is too large, straight control cannot be applied too much on motors.
    int16_t max_straight_component = min(MAX_CONTROL - two_controlls.first, MAX_CONTROL - two_controlls.second);

    int16_t straight_component = min(straight_control, max_straight_component);

    // 可能要调一下 wheel_control 的逻辑？在数据不在范围内时报错一下

    ctrl_record.left_control = two_controlls.first + straight_component;
    ctrl_record.right_control = two_controlls.second + straight_component;
    left_wheel_control(two_controlls.first + straight_component);
    right_wheel_control(two_controlls.second + straight_component);
}

void fullPowerForward() {
    ctrl_record.left_control = MAX_CONTROL;
    ctrl_record.right_control = MAX_CONTROL;
    left_wheel_control(MAX_CONTROL);
    right_wheel_control(MAX_CONTROL);
}

void sharpUTurn(uint16_t setpoint_dist, uint16_t distance, int16_t yaw) {

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
        // left_pwm = ctrl_pwm_map(control, 0, 1000, 55, 255);
        left_pwm = ctrl_pwm_map(control, 0, 1000, 45, 255);
    }
    else {
        // left_pwm = ctrl_pwm_map(control, -1000, 0, -255, -30);
        left_pwm = ctrl_pwm_map(control, -1000, 0, -255, -20);
    }
    left_wheel_move(left_pwm);
}

/* 改动 1：相同的 control 值，右轮跑得比左轮快，所以微调了映射关系（减少了 pwm 映射值上限） */
/* 改动 2：保留了一部分死区，以免小车在 control ~= 0 时也动来动去 */

void right_wheel_control(int16_t control) {
    ctrl_record.right_control = control;
    int16_t right_pwm = 0;
    if (control >= 0) {
        // right_pwm = ctrl_pwm_map(control, 0, 1000, 28, 255);
        right_pwm = ctrl_pwm_map(control, 0, 1000, 18, 180);
    }
    else {
        // right_pwm = ctrl_pwm_map(control, -1000, 0, -255, -28);
        right_pwm = ctrl_pwm_map(control, -1000, 0, -180, -18);
    }
    right_wheel_move(right_pwm);
}

// assume control range from -1000 ~ 1000
// void straight_control(int16_t control) {
//     // map control signal to actual pwm in order to 
//     // eliminate the influence of dead band

//     // formula is:  y = (x - x_min) * (y_max - y_min) / (x_max - x_min) + y_min
//     left_wheel_control(control);
//     right_wheel_control(control);
// }

void stop() {
    left_wheel_move(0);
    right_wheel_move(0);
}