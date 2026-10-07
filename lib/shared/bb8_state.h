#pragma once

enum DriveModes {
    MANUAL, 
    FIGURE8, 
    CIRCLE
};

struct RobotState {
    int des_vel;
    float des_yaw_rate;
    bool enabled;
    float gyro_x, gyro_y, gyro_z;
    float acc_x, acc_y, acc_z;
    float drv_kp, drv_kd;
    float str_kp, str_kd;
    int mode;
};

extern RobotState state;
