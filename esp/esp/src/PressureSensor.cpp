#include "PressureSensor.h"

PressureSensor::PressureSensor(uint8_t pin) {
    _pin = pin;
}

void PressureSensor::begin() {
    pinMode(_pin, INPUT);
}

int PressureSensor::readRaw() {
    return analogRead(_pin);
}

float PressureSensor::readVoltage() {
    int raw = readRaw();
    // Convert 12-bit ADC reading (0-4095) to estimated voltage (0-3.3V)
    return (raw / 4095.0) * 3.3;
}

String PressureSensor::getStatus() {
    int raw = readRaw();
    
    // Adjust these thresholds based on your physical testing with the pillow
    if (raw < 50) {
        return "No Pressure";
    } else if (raw < 1500) {
        return "Light Touch";
    } else if (raw < 3000) {
        return "Medium Press";
    } else {
        return "Heavy Press";
    }
}