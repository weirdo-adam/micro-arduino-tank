//
// Created by L Mr on 2025/8/4.
//

#pragma once
#include <ESP8266WebServer.h>
#include "servo_control.h"
#include "control_html.h"

class WebServerController {
public:
    void begin(ServoController& servoCtrl) {
        // Store reference to servo controller
        this->servoCtrl = &servoCtrl;

        server.on("/", HTTP_GET, [this]() {
            server.send_P(200, "text/html", control_html);
        });

        server.on("/set", HTTP_GET, [this]() {
            if (server.hasArg("rotation")) {
                this->servoCtrl->setRotation(server.arg("rotation").toInt());
                server.send(200, "text/plain", "OK");
            } else if (server.hasArg("elevation")) {
                this->servoCtrl->setElevation(server.arg("elevation").toInt() + 90);
                server.send(200, "text/plain", "OK");
            } else {
                server.send(400, "text/plain", "Bad Request");
            }
        });

        server.on("/fire", HTTP_GET, [this]() {
            this->servoCtrl->fire();

            // Blink LED for fire effect
            for (int i = 0; i < 3; i++) {
                digitalWrite(LED_BUILTIN, LOW);
                delay(100);
                digitalWrite(LED_BUILTIN, HIGH);
                delay(100);
            }

            server.send(200, "text/plain", "FIRED");
        });

        server.on("/ip", HTTP_GET, [this]() {
            server.send(200, "text/plain", WiFi.localIP().toString());
        });

        server.begin();
    }

    void handleClient() {
        server.handleClient();
    }

private:
    ESP8266WebServer server{80};
    ServoController* servoCtrl; // Pointer to servo controller
};