#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>

ESP8266WebServer server(80);

void setup() {
    Serial.begin(115200);
    WiFi.setSleepMode(WIFI_NONE_SLEEP);  // 禁用低功耗模式
    WiFiManager wifiManager;
    wifiManager.autoConnect("ServoController");

    Serial.println("\nWiFi connected!");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());

    server.on("/", []() {
      server.send(200, "text/plain", "Hello World!");
    });
    server.begin();
}

void loop() {
    server.handleClient();
}