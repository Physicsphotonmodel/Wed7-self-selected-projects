#include "pump.h"
#include "Arduino.h"

Pump::Pump(uint8_t pin) {
    _pin = pin;
}

void Pump::begin() {
    pinMode(_pin, OUTPUT);
    off();
}

void Pump::on() {
    digitalWrite(_pin, HIGH);
}

void Pump::off() {
    digitalWrite(_pin, LOW);
}

void Pump::setpwm(int pwm_value) {
    analogWrite(_pin, pwm_value);
}
