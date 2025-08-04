#include <Arduino.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
#include "servo_control.h"
#include "web_server.h"
#include "wifi_manager.h"

ServoController servoController;
WebServerController webServerController;
WiFiController wifiController;

void setup() {
    Serial.begin(115200);
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH);

    servoController.begin();
    wifiController.connect();
    webServerController.begin(servoController);

    Serial.println("HTTP服务器已启动");
    Serial.print("IP地址: ");
    Serial.println(WiFi.localIP());
}

void loop() {
    webServerController.handleClient();
    servoController.update();
    delay(10);
}