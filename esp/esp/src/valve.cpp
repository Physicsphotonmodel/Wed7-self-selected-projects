#include "Arduino.h"
#include "valve.h"

Valve::Valve(uint8_t pin) {
    _pin = pin;
}

// Initialize function
void Valve::begin() {
    pinMode(_pin, OUTPUT);
    close();
}

// Open valve
void Valve::open() {
    digitalWrite(_pin, LOW);
}

// Close valve
void Valve::close() {
    digitalWrite(_pin, HIGH);
}