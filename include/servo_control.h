//
// Created by L Mr on 2025/8/4.
//

#pragma once
#include <EEPROM.h>
#include "Servo.h"
#include "config.h"

class ServoController {
public:
    void begin() {
        turretRotation.attach(TURRET_ROTATION_PIN);
        turretRotation.writeMicroseconds(1500);

        cannonElevation.attach(CANNON_ELEVATION_PIN, MIN_PULSE, MAX_PULSE);
        currentElevationAngle = readElevationFromEEPROM();
        targetElevationAngle = currentElevationAngle;
        cannonElevation.write(currentElevationAngle);
    }

    void update() {
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

            if (currentElevationAngle == targetElevationAngle) {
                saveElevationToEEPROM(currentElevationAngle);
            }
        }
    }

    void fire() {
        targetRotationSpeed = 0;
        currentRotationSpeed = 0;
        turretRotation.writeMicroseconds(1500);
    }

    void setRotation(int speed) { targetRotationSpeed = speed; }
    void setElevation(int angle) { targetElevationAngle = constrain(angle, MIN_ELEVATION_ANGLE, MAX_ELEVATION_ANGLE); }

private:
    Servo turretRotation;
    Servo cannonElevation;
    int targetRotationSpeed = 0;
    int currentRotationSpeed = 0;
    int targetElevationAngle = 90;
    int currentElevationAngle = 90;

    void saveElevationToEEPROM(int angle) {
        EEPROM.begin(EEPROM_SIZE);
        EEPROM.put(0, angle);
        EEPROM.commit();
        EEPROM.end();
    }

    int readElevationFromEEPROM() {
        EEPROM.begin(EEPROM_SIZE);
        int angle;
        EEPROM.get(0, angle);
        EEPROM.end();
        return constrain(angle, MIN_ELEVATION_ANGLE, MAX_ELEVATION_ANGLE);
    }
};
