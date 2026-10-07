#include <Arduino.h>
#include <WiFi.h>
#include "web_server.h"
#include <LittleFS.h>
#include "bb8_state.h"
#include <ArduinoJson.h>
#include "secrets.h"

void handleWsEvent(AsyncWebSocket *server, 
                   AsyncWebSocketClient *client, 
                   AwsEventType type, 
                   void *arg, 
                   uint8_t *data, 
                   size_t len) {
    if (type == WS_EVT_CONNECT) {
        Serial.println("WS client connected");
    }
    if (type == WS_EVT_DATA) {
        AwsFrameInfo *info = (AwsFrameInfo*)arg;

        if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
            data[len] = 0;
            String msg = (char*)data;

            StaticJsonDocument<256> doc;
            DeserializationError err = deserializeJson(doc, msg);
            if (err) {
                Serial.println("JSON parse failed");
                return;
            }

            const char* cmd = doc["cmd"];

            if (strcmp(cmd, "drive_mode") == 0) {
                if (state.mode == DriveModes::MANUAL) 
                    state.mode = DriveModes::FIGURE8;
                else 
                    state.mode = DriveModes::MANUAL;
            }

            if (strcmp(cmd, "drv_speed") == 0 && state.enabled) {
                state.des_vel = doc["value"].as<int>();
            }

            if (strcmp(cmd, "fw_speed") == 0 && state.enabled) {
                state.des_yaw_rate = doc["value"].as<int>();
            }

            if (strcmp(cmd, "kd") == 0 && state.enabled) {
                state.drv_kd = doc["value"].as<int>();
            }

            if (strcmp(cmd, "kp") == 0 && state.enabled) {
                state.drv_kp = doc["value"].as<int>();
            }
        }
    }
}


int Srv::begin() {
    WiFi.begin(WIFI_SSID, WIFI_PASS);

    Serial.print("Connecting");

    int i = 0;
    while (WiFi.status() != WL_CONNECTED && i < 30) {
        delay(500);
        Serial.print(".");
        i += 1;
    }
    Serial.println();

    if (i == 30) {
        return -1;
    }
    else {
        Serial.print("Connected to ");
        Serial.println(WIFI_SSID);
    }

    Serial.print("IP: ");
    Serial.println(WiFi.localIP());

    if (!LittleFS.begin()) {
        Serial.println("Failed to mount LittleFS");
        return -2;
    }

    server.serveStatic("/", LittleFS, "/").setDefaultFile("index.html");

    // DEBUG: Not sure if i need this GET handler
    server.on("/api/speed", HTTP_GET, [](AsyncWebServerRequest *request) {
        if (request->hasParam("value")) {
            int speed = request->getParam(0)->value().toInt();
            state.des_vel = speed;
            request->send(200, "text/plain", "ok");
        }
    });

    server.on("/api/stop", HTTP_GET, [](AsyncWebServerRequest *request) {
        state.enabled = false;
    });

    server.on("/api/enable", HTTP_GET, [](AsyncWebServerRequest *request) {
        state.enabled = true;
    });
    
    server.begin();

    ws.onEvent(handleWsEvent);
    server.addHandler(&ws);
    
    return 0;
}

void Srv::update() {
    // Send a JSON object through the socket
    JsonDocument doc;
    doc["angle"] = state.acc_y;
    String buf;
    serializeJson(doc, buf);
    ws.textAll(buf);
}
