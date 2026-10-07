#include <Arduino.h>
#include "fw_motor.h"
#include "bb8_state.h"
#include <ESP32Servo.h>
#include <cmath>


Servo fwESC;
int tiempo; // should probably have actual time type, will try this for now
void update8();


void FWMotor::begin() {
    fwESC.attach(FW_PIN);
    fwESC.write(MID_THR); // middle position to arm
    Serial.println("Arming flywheel motor");
    tiempo = 0;
    delay(3000); // 3 second delay to arm i think
}

void FWMotor::update() {
    if (state.enabled) {
        if (state.mode == DriveModes::MANUAL)
            fwESC.write(state.des_yaw_rate);
        else 
            update8();
        return;
    }
    fwESC.write(MID_THR);
}

void update8() {
    float freq = 0.5f;
    float amp = 50.0f;
    float fwSpeed = amp * sin(2 * PI * freq * tiempo);
    fwESC.write(fwSpeed);
    tiempo += 0.005; // this is the delay in the mainloop, likely will be >5ms in practice
}
