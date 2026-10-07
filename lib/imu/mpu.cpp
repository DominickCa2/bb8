#include "mpu.h"
#include <Wire.h>
#include <Arduino.h>
#include "bb8_state.h"


int check_err(int err) {
    if (err != 0) {
        Serial.printf("Error: %d\n", err);
        state.des_vel = 0;
        state.des_vel = 0;

        Serial.println("Reinitializing I2C...");
        Wire.end();
        Wire.begin(SDA, SCL);
        Wire.setClock(100000);
        return -1;
    }
    return 0;
}

// Take the MPU out of low power sleep mode
void wake_up() {
    Wire.beginTransmission(ADDR);
    Wire.write(0x6B); 
    Wire.write(0x00);
    Wire.endTransmission(true);
}

// Convert raw gyro and acc values
void convert(uint8_t *acc, uint8_t *gyro) {
    // Acc stuff
    short acc_x_raw = (acc[0] << 8) | acc[1];
    short acc_y_raw = (acc[2] << 8) | acc[3];
    short acc_z_raw = (acc[4] << 8) | acc[5];

    state.acc_x = acc_x_raw / 16384.0;
    state.acc_y = acc_y_raw / 16384.0;
    state.acc_z = acc_z_raw / 16384.0;

    // Gyro stuff
    short gyro_x_raw = (gyro[0] << 8) | gyro[1];
    short gyro_y_raw = (gyro[2] << 8) | gyro[3];
    short gyro_z_raw = (gyro[4] << 8) | gyro[5];

    state.gyro_x = (gyro_x_raw / 32768.0) * 250.0;
    state.gyro_y = (gyro_y_raw / 32768.0) * 250.0;
    state.gyro_z = (gyro_z_raw / 32768.0) * 250.0;

    // DEBUG
    // Serial.printf("gyro_x: %.2f, gyro_y: %.2f, gyro_z: %.2f\n", state.gyro_x, state.gyro_y, state.gyro_z);
    // Serial.printf("acc_x: %.2f, acc_y: %.2f, acc_z: %.2f\n", state.acc_x, state.acc_y, state.acc_z);
}

void MPU::begin() {
    Wire.begin(SDA, SCL);
    Wire.setClock(100000); // Set to lower sample rate cuz it was cutting out earlier
    wake_up();
    delay(100); 
}

void MPU::update() {
    Wire.beginTransmission(ADDR); 
    Wire.write(0x3B); 
    int err = Wire.endTransmission(false); 

    // Each value is 2 bytes
    // Acc data on addresses 0x3B - 0x40
    err = Wire.requestFrom(ADDR, 6);

    uint8_t acc[6];
    Wire.readBytes(acc, 6);

    // Accel data is on addresses 0x43 - 0x48
    Wire.beginTransmission(ADDR);
    Wire.write(0x43);
    err = Wire.endTransmission(false);

    err = Wire.requestFrom(ADDR, 6);

    uint8_t gyro[6];
    Wire.readBytes(gyro, 6);

    // DEBUG: Wire.endTransmission() only used after writing to the I2C bus
    // Wire.endTransmission();

    convert(acc, gyro);
}

