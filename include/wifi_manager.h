//
// Created by L Mr on 2025/8/4.
//

#pragma once
#include <WiFiManager.h>
#include "config.h"

class WiFiController {
public:
    void connect() {
        WiFiManager wifiManager;
        wifiManager.autoConnect(WIFI_SSID);
    }
};