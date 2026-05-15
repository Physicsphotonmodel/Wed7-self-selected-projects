#include "driver.h"

Driver::Driver(uint8_t pin) {
    _pin = pin;
}

void Driver::begin() {
    pinMode(_pin, OUTPUT);
    off();
}

void Driver::on() {
    digitalWrite(_pin, HIGH);
}

void Driver::off() {
    digitalWrite(_pin, LOW);
}

void Driver::setIntensity(uint8_t percent) {
    if (percent > 100) percent = 100;
    if (percent < 0) percent = 0;

    uint8_t pwmValue = map(percent, 0, 100, 0, 255);

    analogWrite(_pin, pwmValue);
}