#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
#include <Servo.h>

const int SERVO_PIN = D4;
const int LED_PIN = D0;

Servo servo;
ESP8266WebServer server(80);

// 舵机速度控制参数
int targetSpeed = 0; // -100 到 100
int currentSpeed = 0; // 当前实际速度
const int ACCELERATION = 5; // 加速度（每步变化量）

// 立即停止舵机
void stopServo() {
    servo.writeMicroseconds(1500);
    currentSpeed = 0;
}

void setup() {
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, HIGH); // LED亮表示未连接

    Serial.begin(115200);
    delay(1000);

    // 初始化舵机（360度连续旋转模式）
    servo.attach(SERVO_PIN);
    stopServo(); // 初始状态停止

    // WiFi连接（使用WiFiManager）
    WiFiManager wifiManager;
    wifiManager.autoConnect("ServoSpeedController");

    Serial.println("\nConnected to WiFi!");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
    digitalWrite(LED_PIN, LOW); // LED灭表示已连接

    // 网页控制界面
    server.on("/", HTTP_GET, []() {
        String html = R"=====(
<!DOCTYPE html><html><head>
<meta charset='UTF-8'>
<meta name='viewport' content='width=device-width, initial-scale=1.0'>
<title>360° Servo Control</title>
<style>
  body { font-family:Arial; text-align:center; margin:20px; }
  .slider-container { width:80%; margin:0 auto; }
  input[type='range'] { width:100%; }
  .speed-display { font-size:24px; margin:15px; }
  .btn { padding:10px 20px; margin:5px; }
</style>
</head>
<body>
<h1>360° Servo Speed Control</h1>
<div class='slider-container'>
  <input type='range' min='-100' max='100' value='0' id='speedSlider'>
  <div class='speed-display'>Speed: <span id='speedValue'>0</span>%</div>
  <button class='btn' onclick='setSpeed(0)'>STOP</button>
</div>
<script>
const slider = document.getElementById('speedSlider');
const speedValue = document.getElementById('speedValue');

slider.oninput = function() {
  speedValue.textContent = this.value;
  fetch('/set?speed=' + this.value);
}

function setSpeed(speed) {
  slider.value = speed;
  speedValue.textContent = speed;
  fetch('/set?speed=' + speed);
}
</script>
</body></html>
)=====";
        server.send(200, "text/html", html);
    });

    server.on("/set", HTTP_GET, []() {
        if (server.hasArg("speed")) {
            targetSpeed = server.arg("speed").toInt();
            targetSpeed = constrain(targetSpeed, -100, 100);
            Serial.printf("Set target speed: %d%%\n", targetSpeed);
            server.send(200, "text/plain", "OK");
        } else {
            server.send(400, "text/plain", "Missing speed parameter");
        }
    });

    server.begin();
    Serial.println("HTTP server started");
}

// 设置舵机速度（-100到100）
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

    servo.writeMicroseconds(pulseWidth);
}

// 平滑速度过渡
void updateSpeed() {
    if (currentSpeed != targetSpeed) {
        int step = (targetSpeed > currentSpeed) ? ACCELERATION : -ACCELERATION;
        currentSpeed += step;

        // 防止超调
        if ((step > 0 && currentSpeed > targetSpeed) ||
            (step < 0 && currentSpeed < targetSpeed)) {
            currentSpeed = targetSpeed;
        }

        setServoSpeed(currentSpeed);
        Serial.printf("Current speed: %d%%\n", currentSpeed);
    }
}

void loop() {
    server.handleClient();
    updateSpeed(); // 平滑更新速度
    delay(20); // 控制循环频率
}
