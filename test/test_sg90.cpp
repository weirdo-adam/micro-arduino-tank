#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
#include <Servo.h>

const int SERVO_360_PIN = D4; // 360度舵机引脚
const int SERVO_180_PIN = D5; // 180度舵机引脚

Servo servo360; // 360度连续旋转舵机
Servo servo180; // 180度标准舵机
ESP8266WebServer server(80);

// 360度舵机控制参数
int targetSpeed = 0; // -100 到 100
int currentSpeed = 0; // 当前实际速度
const int ACCELERATION = 5; // 加速度（每步变化量）

// 180度舵机控制参数
int targetAngle = 90; // 0 到 180
int currentAngle = 90; // 当前角度
const int ANGLE_STEP = 2; // 角度变化步长

// 扩展180度舵机角度范围参数
const int MIN_PULSE = 500; // 最小脉冲宽度(µs)
const int MAX_PULSE = 2500; // 最大脉冲宽度(µs)
const int EXTENDED_MIN_ANGLE = 0; // 显示的最小角度
const int EXTENDED_MAX_ANGLE = 270; // 显示的最大角度（实际物理角度可能达不到）

const char *WIFI_SSID = "ESP-8266-DualServoTest"; // WIFI Name

// 360度舵机控制函数
void stopServo360() {
    servo360.writeMicroseconds(1500);
    currentSpeed = 0;
}

void setServoSpeed(int speed) {
    speed = constrain(speed, -100, 100);
    int pulseWidth;

    if (speed == 0) {
        pulseWidth = 1500; // 停止
    } else if (speed > 0) {
        pulseWidth = map(speed, 1, 100, 1530, 2000); // 顺时针加速
    } else {
        pulseWidth = map(speed, -1, -100, 1470, 1000); // 逆时针加速
    }

    servo360.writeMicroseconds(pulseWidth);
}

// 180度舵机控制函数（扩展角度范围）
void setServoAngle(int angle) {
    angle = constrain(angle, EXTENDED_MIN_ANGLE, EXTENDED_MAX_ANGLE);
    // 将扩展角度映射到实际脉冲宽度
    int pulseWidth = map(angle, EXTENDED_MIN_ANGLE, EXTENDED_MAX_ANGLE, MIN_PULSE, MAX_PULSE);
    servo180.writeMicroseconds(pulseWidth);
}

void updateSpeed() {
    if (currentSpeed != targetSpeed) {
        int step = (targetSpeed > currentSpeed) ? ACCELERATION : -ACCELERATION;
        currentSpeed += step;
        if ((step > 0 && currentSpeed > targetSpeed) || (step < 0 && currentSpeed < targetSpeed)) {
            currentSpeed = targetSpeed;
        }
        setServoSpeed(currentSpeed);
    }
}

void updateAngle() {
    if (currentAngle != targetAngle) {
        int step = (targetAngle > currentAngle) ? ANGLE_STEP : -ANGLE_STEP;
        currentAngle += step;
        if ((step > 0 && currentAngle > targetAngle) || (step < 0 && currentAngle < targetAngle)) {
            currentAngle = targetAngle;
        }
        setServoAngle(currentAngle);
    }
}

void setup() {

    Serial.begin(115200);
    delay(1000);

    // 初始化舵机
    servo360.attach(SERVO_360_PIN);
    stopServo360(); // 初始状态停止

    // 初始化180度舵机，设置扩展的脉冲宽度范围
    servo180.attach(SERVO_180_PIN, MIN_PULSE, MAX_PULSE);
    setServoAngle(currentAngle); // 初始位置90度

    // WiFi连接（使用WiFiManager）
    WiFiManager wifiManager;
    wifiManager.autoConnect(WIFI_SSID);

    Serial.println("\nConnected to WiFi!");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
    // 网页控制界面
    server.on("/", HTTP_GET, []() {
        // HTML头部
        String html = "<!DOCTYPE html><html><head>";
        html += "<meta charset='UTF-8'><meta name='viewport' content='width=device-width, initial-scale=1.0'>";
        html += "<title>Dual Servo Controller</title>";
        html += "<style>body{font-family:Arial;text-align:center;margin:20px;}";
        html += ".control-panel{max-width:600px;margin:0 auto;padding:20px;border:1px solid #ddd;border-radius:10px;}";
        html += ".servo-control{margin-bottom:30px;}";
        html += ".slider-container{width:90%;margin:15px auto;}";
        html += "input[type='range']{width:100%;}";
        html += ".value-display{font-size:24px;margin:15px;}";
        html += ".btn{padding:10px 20px;margin:5px;background:#4CAF50;color:white;border:none;border-radius:4px;}";
        html += ".btn-stop{background:#f44336;}.btn-reset{background:#2196F3;}";
        html += "h2{color:#333;border-bottom:1px solid #eee;padding-bottom:10px;}</style>";
        html += "</head><body><div class='control-panel'><h1>Dual Servo Controller</h1>";

        // 360度舵机控制部分
        html += "<div class='servo-control'><h2>360° Continuous Rotation Servo</h2>";
        html += "<div class='slider-container'>";
        html += "<input type='range' min='-100' max='100' value='0' id='speedSlider'>";
        html += "<div class='value-display'>Speed: <span id='speedValue'>0</span>%</div>";
        html += "<button class='btn btn-stop' onclick='setSpeed(0)'>STOP</button></div></div>";

        // 180度舵机控制部分（扩展角度范围）
        html += "<div class='servo-control'><h2>Extended Range Servo (0-270°)</h2>";
        html += "<div class='slider-container'>";
        html += "<input type='range' min='0' max='270' value='90' id='angleSlider'>";
        html += "<div class='value-display'>Angle: <span id='angleValue'>90</span>°</div>";
        html += "<button class='btn btn-reset' onclick='setAngle(135)'>CENTER (135°)</button></div></div>";

        // JavaScript部分
        html += "<script>";
        html += "const speedSlider=document.getElementById('speedSlider');";
        html += "const speedValue=document.getElementById('speedValue');";
        html += "speedSlider.oninput=function(){speedValue.textContent=this.value;fetch('/set?speed='+this.value);};";
        html +=
                "function setSpeed(speed){speedSlider.value=speed;speedValue.textContent=speed;fetch('/set?speed='+speed);}";

        html += "const angleSlider=document.getElementById('angleSlider');";
        html += "const angleValue=document.getElementById('angleValue');";
        html += "angleSlider.oninput=function(){angleValue.textContent=this.value;fetch('/set?angle='+this.value);};";
        html +=
                "function setAngle(angle){angleSlider.value=angle;angleValue.textContent=angle;fetch('/set?angle='+angle);}";
        html += "</script></body></html>";

        server.send(200, "text/html", html);
    });

    server.on("/set", HTTP_GET, []() {
        if (server.hasArg("speed")) {
            targetSpeed = server.arg("speed").toInt();
            targetSpeed = constrain(targetSpeed, -100, 100);
            server.send(200, "text/plain", "OK");
        } else if (server.hasArg("angle")) {
            targetAngle = server.arg("angle").toInt();
            targetAngle = constrain(targetAngle, EXTENDED_MIN_ANGLE, EXTENDED_MAX_ANGLE);
            server.send(200, "text/plain", "OK");
        } else {
            server.send(400, "text/plain", "Missing parameter");
        }
    });

    server.begin();
    Serial.println("HTTP server started");
}

void loop() {
    server.handleClient();
    updateSpeed(); // 更新360度舵机速度
    updateAngle(); // 更新180度舵机角度
    delay(20); // 控制循环频率
}
