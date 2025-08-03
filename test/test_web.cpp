#include <Arduino.h>
#include <Servo.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>

const int SERVO_PIN = D4;
const int LED_PIN = D0;

Servo servo;
ESP8266WebServer server(80);

// 舵机控制参数
int targetAngle = 90;
int currentAngle = 90;
unsigned long lastMoveTime = 0;
const int MIN_ANGLE_CHANGE = 3;    // 最小有效角度变化量
const int MOVE_INTERVAL = 20;      // 运动间隔(ms)

// WiFi连接参数
const char* AP_NAME = "ServoController";
const unsigned long WIFI_CHECK_INTERVAL = 5000;

void setup() {
  // 初始化硬件
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);  // LED亮表示未连接
  Serial.begin(115200);
  delay(1000);

  // 初始化舵机
  servo.attach(SERVO_PIN);
  servo.write(currentAngle);

  // 连接WiFi
  WiFiManager wifiManager;
  wifiManager.setConnectTimeout(30);      // 30秒连接超时
  wifiManager.setConfigPortalTimeout(180); // 3分钟配置超时

  Serial.println("Connecting to WiFi...");
  if (!wifiManager.autoConnect(AP_NAME)) {
    Serial.println("Failed to connect");
    digitalWrite(LED_PIN, HIGH);  // 保持LED亮表示失败
    delay(3000);
    ESP.restart();
  }

  Serial.println("\nWiFi connected!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  digitalWrite(LED_PIN, LOW);  // LED灭表示已连接

  // 设置Web服务器
  server.on("/", HTTP_GET, []() {
    String html = R"=====(
<!DOCTYPE html><html><head>
<meta charset='UTF-8'>
<meta name='viewport' content='width=device-width, initial-scale=1.0'>
<title>Servo Controller</title>
<style>
  body { font-family:Arial; text-align:center; margin:20px; }
  input { width:80%; margin:20px 0; }
  .status { color: #666; font-size:0.9em; margin-top:20px; }
</style>
</head>
<body>
<h1>Servo Controller</h1>
<p>Current Angle: <span id='angle'>)=====";
    html += currentAngle;
    html += R"=====(</span>°</p>
<input type='range' min='0' max='180' value=')=====";
    html += currentAngle;
    html += R"=====(' id='slider'>
<p class='status' id='status'>Ready</p>
<script>
let debounceTimer;
slider.oninput = function() {
  document.getElementById('angle').innerHTML = this.value;
  document.getElementById('status').innerHTML = 'Adjusting...';
  clearTimeout(debounceTimer);
  debounceTimer = setTimeout(() => {
    fetch('/set?angle='+this.value)
      .then(r => document.getElementById('status').innerHTML = 'Updated')
      .catch(e => document.getElementById('status').innerHTML = 'Error');
  }, 150);
}
</script>
</body></html>
)=====";
    server.send(200, "text/html", html);
  });

  server.on("/set", HTTP_GET, []() {
    if (server.hasArg("angle")) {
      int newAngle = constrain(server.arg("angle").toInt(), 0, 180);

      // 只有角度变化足够大时才更新目标
      if (abs(newAngle - targetAngle) >= MIN_ANGLE_CHANGE) {
        targetAngle = newAngle;
        Serial.printf("New target angle: %d\n", targetAngle);
      }
      server.send(200, "text/plain", String(currentAngle));
    } else {
      server.send(400, "text/plain", "Missing angle parameter");
    }
  });

  server.begin();
  Serial.println("HTTP server started");
}

void smoothMove() {
  static unsigned long lastMove = 0;

  if (millis() - lastMove >= MOVE_INTERVAL) {
    if (currentAngle != targetAngle) {
      // 每次移动1度，实现平滑运动
      currentAngle += (targetAngle > currentAngle) ? 1 : -1;
      servo.write(currentAngle);
      lastMove = millis();
    }
  }
}

void checkWiFi() {
  static unsigned long lastCheck = 0;

  if (millis() - lastCheck >= WIFI_CHECK_INTERVAL) {
    if (WiFi.status() != WL_CONNECTED) {
      digitalWrite(LED_PIN, HIGH);
      Serial.println("WiFi disconnected! Reconnecting...");
      WiFi.reconnect();
      delay(100); // 短暂延迟让reconnect开始
    }
    lastCheck = millis();
  }
}

void loop() {
  server.handleClient();
  smoothMove();
  checkWiFi();
}