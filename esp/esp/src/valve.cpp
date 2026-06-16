#include "Arduino.h"
#include "valve.h"

Valve::Valve(uint8_t pin) {
    _pin = pin;
}

// 初始化函式
void Valve::begin() {
    pinMode(_pin, OUTPUT);
    close();
}

// 開啟閥門
void Valve::open() {
    digitalWrite(_pin, LOW);
}

// 關閉閥門
void Valve::close() {
    digitalWrite(_pin, HIGH);
}