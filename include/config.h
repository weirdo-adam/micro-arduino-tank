//
// Created by L Mr on 2025/8/4.
//
#pragma once

// 引脚定义
const int TURRET_ROTATION_PIN = D4; // 炮台旋转舵机引脚
const int CANNON_ELEVATION_PIN = D5; // 炮筒俯仰舵机引脚

// 控制参数
const int ROTATION_ACCELERATION = 3;
const int ELEVATION_STEP = 1;
const int EEPROM_SIZE = 4;
const int MIN_PULSE = 500;
const int MAX_PULSE = 2500;
const int MIN_ELEVATION_ANGLE = 30;
const int MAX_ELEVATION_ANGLE = 150;
const char *WIFI_SSID = "Tank-Control-System";