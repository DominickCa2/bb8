#include <Arduino.h>
#include "fw_motor.h"
#include "bb8_state.h"
#include <ESP32Servo.h>

Servo fwServo;

void FWMotor::begin() {
    fwServo.attach(FW_PIN);
    fwServo.write(MID_THR); // middle position to arm
    Serial.println("Arming flywheel motor");
    delay(3000); // 3 second delay to arm i think
}

void FWMotor::update() {
    if (state.enabled) {
        fwServo.write(state.des_yaw_rate);
        return;
    }
    fwServo.write(MID_THR);
}
