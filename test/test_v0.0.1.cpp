#include <ESP8266WebServer.h>
#include <WiFiManager.h>
#include <Servo.h>
#include <EEPROM.h>
#include <iostream>
#include <ostream>

// 引脚定义
const int TURRET_ROTATION_PIN = D4; // 炮台旋转舵机引脚
const int CANNON_ELEVATION_PIN = D5; // 炮筒俯仰舵机引脚

// 伺服对象
Servo turretRotation; // 炮台旋转舵机
Servo cannonElevation; // 炮筒俯仰舵机
ESP8266WebServer server(80);

// 控制参数
int targetRotationSpeed = 0;
int currentRotationSpeed = 0;
int targetElevationAngle = 90;
int currentElevationAngle = 90;

// 常量定义
const int ROTATION_ACCELERATION = 3;
const int ELEVATION_STEP = 1;
const int EEPROM_SIZE = 4;
const int MIN_PULSE = 500;
const int MAX_PULSE = 2500;
const int MIN_ELEVATION_ANGLE = 30;
const int MAX_ELEVATION_ANGLE = 150;
const char *WIFI_SSID = "Tank-Control-System";

// 保存角度到EEPROM
void saveElevationToEEPROM(int angle) {
    EEPROM.begin(EEPROM_SIZE);
    EEPROM.put(0, angle);
    EEPROM.commit();
    EEPROM.end();
}

// 从EEPROM读取角度
int readElevationFromEEPROM() {
    EEPROM.begin(EEPROM_SIZE);
    int angle;
    EEPROM.get(0, angle);
    EEPROM.end();
    return constrain(angle, MIN_ELEVATION_ANGLE, MAX_ELEVATION_ANGLE);
}

void setup() {
    Serial.begin(115200);
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH); // 初始状态关闭

    // 初始化舵机
    turretRotation.attach(TURRET_ROTATION_PIN);
    turretRotation.writeMicroseconds(1500); // 停止

    cannonElevation.attach(CANNON_ELEVATION_PIN, MIN_PULSE, MAX_PULSE);
    currentElevationAngle = readElevationFromEEPROM();
    cannonElevation.write(currentElevationAngle);

    // WiFi连接
    WiFiManager wifiManager;
    wifiManager.autoConnect(WIFI_SSID);

    // 注册HTTP处理函数
    server.on("/", HTTP_GET, []() {
        String html = "<!DOCTYPE html><html><head><meta name='viewport' content='width=device-width,initial-scale=1'>";
        html += "<title>Tank Control</title><style>";
        html += "body{font-family:Arial;margin:10px;background:#f0f0f0;}";
        html += ".btn{display:block;width:100%;padding:10px;margin:5px 0;border:none;color:white;font-weight:bold;}";
        html += ".fire{background:#f80;}";
        html += ".stop{background:#444;}";
        html += ".slider{width:100%;margin:10px 0;}";
        html += ".value{text-align:center;font-size:1.2em;margin:5px 0;}";
        html += "</style></head><body>";

        html += "<h1>Tank Control</h1>";


        // 旋转控制
        html += "<h2>Rotation</h2>";
        html += "<input type='range' class='slider' min='-100' max='100' value='0' id='rot'>";
        html += "<div class='value'>Speed: <span id='rotVal'>0</span>%</div>";
        html += "<button class='btn stop' onclick=\"setRot(0)\">STOP</button>";

        // 俯仰控制
        html += "<h2>Elevation</h2>";
        html += "<input type='range' class='slider' min='30' max='150' value='" + String(currentElevationAngle) +
                "' id='elev'>";
        html += "<div class='value'>Angle: <span id='elevVal'>" + String(currentElevationAngle) +
                "</span>&deg;</div>";

        // 开火控制
        html += "<h2>Fire</h2>";
        html += "<button class='btn fire' onclick=\"fetch('/fire')\">FIRE</button>";


        html += "<script>";
        html += "function setRot(v){";
        html += "document.getElementById('rot').value=v;";
        html += "document.getElementById('rotVal').innerText=v;";
        html += "fetch('/set?rotation='+v);}";

        html += "function setElev(v){";
        html += "document.getElementById('elev').value=v;";
        html += "document.getElementById('elevVal').innerText=v;";
        html += "fetch('/set?elevation='+v);}";

        html += "document.getElementById('rot').addEventListener('input',function(){";
        html += "setRot(this.value);});";

        html += "document.getElementById('elev').addEventListener('input',function(){";
        html += "setElev(this.value);});";
        html += "</script></body></html>";

        server.send(200, "text/html", html);
    });

    server.on("/set", HTTP_GET, []() {
        if (server.hasArg("rotation")) {
            targetRotationSpeed = server.arg("rotation").toInt();
            server.send(200, "text/plain", "OK");
        } else if (server.hasArg("elevation")) {
            targetElevationAngle = server.arg("elevation").toInt();
            server.send(200, "text/plain", "OK");
        } else {
            server.send(400, "text/plain", "Bad Request");
        }
    });

    server.on("/fire", HTTP_GET, []() {
        currentRotationSpeed = 0; // 跳过减速过程
        turretRotation.writeMicroseconds(1500); // 直接发送停止信号
        // 简单闪灯效果
        for (int i = 0; i < 3; i++) {
            digitalWrite(LED_BUILTIN, LOW);
            delay(150);
            digitalWrite(LED_BUILTIN, HIGH);
            delay(150);
        }
        server.send(200, "text/plain", "FIRED");
    });

    server.begin();
}

void loop() {
    server.handleClient();

    // 更新炮台旋转速度
    if (currentRotationSpeed != targetRotationSpeed) {
        int step = (targetRotationSpeed > currentRotationSpeed)
                       ? min(ROTATION_ACCELERATION, targetRotationSpeed - currentRotationSpeed)
                       : -min(ROTATION_ACCELERATION, currentRotationSpeed - targetRotationSpeed);

        currentRotationSpeed += step;
        int us = 1500 + currentRotationSpeed * 2;
        turretRotation.writeMicroseconds(constrain(us, 1300, 1700));
    }

    // 更新炮筒俯仰角度
    if (currentElevationAngle != targetElevationAngle) {
        int step = (targetElevationAngle > currentElevationAngle)
                       ? min(ELEVATION_STEP, targetElevationAngle - currentElevationAngle)
                       : -min(ELEVATION_STEP, currentElevationAngle - targetElevationAngle);

        currentElevationAngle += step;
        cannonElevation.write(currentElevationAngle);

        // 到达目标位置后保存
        if (currentElevationAngle == targetElevationAngle) {
            saveElevationToEEPROM(currentElevationAngle);
        }
    }

    delay(10);
}
