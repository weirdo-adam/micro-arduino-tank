#include <Arduino.h>
#include <Servo.h>

Servo myservo;  // 创建 Servo 对象
const int servoPin = D4;  // 舵机信号线连接 D4 (GPIO2)

void setup() {
    Serial.begin(115200);
    myservo.attach(servoPin);  // 绑定舵机到指定引脚
    Serial.println("ESP8266 Servo Control (PlatformIO + CLion)");
}

void loop() {
    // 0° → 180° 旋转
    for (int angle = 0; angle <= 180; angle += 30) {
        myservo.write(angle);
        Serial.printf("Angle: %d°\n", angle);
        delay(500);
    }

    // 180° → 0° 旋转
    for (int angle = 180; angle >= 0; angle -= 30) {
        myservo.write(angle);
        Serial.printf("Angle: %d°\n", angle);
        delay(500);
    }
}