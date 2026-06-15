#include "PressureSensor.h"

PressureSensor::PressureSensor(uint8_t pin) : _pin(pin) {}

void PressureSensor::begin() {
    pinMode(_pin, INPUT);
}

int PressureSensor::readRaw() {
    return analogRead(_pin);
}

bool PressureSensor::isPressed() {
    return readRaw() > 3000;
}

