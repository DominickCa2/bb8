#include <Arduino.h>
#include "drive_motor.h"
#include "web_server.h"
#include "bb8_state.h"
#include "mpu.h"
#include "fw_motor.h"
#include "drive_mode.h"

DriveMotor drv;
Srv srv;
MPU mpu;
FWMotor fw;
DriveMode mode;

// important stuff: 
// acc_y
// gyro_y

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  drv.begin();
  mpu.begin();
  fw.begin();
  state.enabled = false;

  int srv_status = srv.begin();
  if (srv_status != 0) {
    if (srv_status == -1)
      Serial.println("Could not connect to Wifi");
    if (srv_status == -2)
      Serial.println("Failed to mount LittleFS");

    while (true) {
      delay(1000);
    }
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  if (!state.enabled) {
    state.des_vel = 0;
    state.des_yaw_rate = 90;
  }
  mpu.update();
  drv.update();
  fw.update();
  srv.update();
  delay(5);
}
