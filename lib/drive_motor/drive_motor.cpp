#include <Arduino.h>
#include "drive_motor.h"
#include "bb8_state.h"

int prev_thr = 0; // Need this cuz changing directions is weird (need digitalWrite(0) for both thr pins before changing dir)

// Maybe eventually have PID in its own controls module
void PID(int32_t *thr_out, int32_t *steer_out) {
    // Stop state
    if (abs(state.des_vel) < 10) {
        // Just try to maintain a 0 degree angle (for acc_y)
        float angle_y = state.acc_y;
        float rot_z = state.gyro_z;

        *thr_out = angle_y * state.drv_kp + rot_z * state.drv_kd;
        Serial.printf("angle_y: %.2f, rot_z: %.2f\n", angle_y, rot_z);
    }
    else {
        *thr_out = state.des_vel;
    }
    *steer_out = 0;
}

void DriveMotor::begin() {
    pinMode(DRV_PIN1, OUTPUT);
    pinMode(DRV_PIN2, OUTPUT);
}

void DriveMotor::update() {
    if (!state.enabled) {
        analogWrite(DRV_PIN1, 0);
        analogWrite(DRV_PIN2, 0);
        digitalWrite(DRV_PIN1, LOW);
        digitalWrite(DRV_PIN2, LOW);   
    }
    else {
        int32_t thr, str;
        PID(&thr, &str);
        Serial.println(thr);
        if (thr > 0) {
            if (prev_thr <= 0) {
                digitalWrite(DRV_PIN1, LOW);
                digitalWrite(DRV_PIN2, LOW);
            }
            else {
                digitalWrite(DRV_PIN2, LOW);
                analogWrite(DRV_PIN1, min(thr, THR_LIMIT));
            }
        }
        else {
            if (prev_thr >= 0) {
                digitalWrite(DRV_PIN1, LOW);
                digitalWrite(DRV_PIN2, LOW);
            }
            else {
                digitalWrite(DRV_PIN1, LOW);
                analogWrite(DRV_PIN2, min(thr * -1, THR_LIMIT));
            }
        }
        prev_thr = thr;
    }
}
