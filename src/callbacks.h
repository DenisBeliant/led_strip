#ifndef CALLBACKS_H
#define CALLBACKS_H
#include "NimBLEDevice.h"
#include <Arduino.h>
#include "var.h"

class CharTypeCallback: public NimBLECharacteristicCallbacks {
    void onRead(NimBLECharacteristic* pCharacteristic){
    };

    void onWrite(NimBLECharacteristic* pCharacteristic) {
        const char *value = pCharacteristic->getValue().c_str();
        mode = atoi(value);
        pCharacteristic->setValue(mode);
        color_me(color, 0, NUM_LEDS);

    };
};

class CharColorCallback: public NimBLECharacteristicCallbacks {
    void onRead(NimBLECharacteristic* pCharacteristic){
        Serial.println("read colors");
    };

    void onWrite(NimBLECharacteristic* pCharacteristic) {
        const char *value = pCharacteristic->getValue().c_str();
        Serial.println(value);
        color = atoi(value);
        Serial.printf("color : %u\n", color);
        color_me(color, 0, NUM_LEDS);

    };
};

#endif