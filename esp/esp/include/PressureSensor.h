#ifndef PRESSURE_SENSOR_H
#define PRESSURE_SENSOR_H

#include <Arduino.h>

class PressureSensor {
private:
    uint8_t _pin;

public:
    // Constructor: Assign the ADC pin
    PressureSensor(uint8_t pin);

    // Initialize the pin mode
    void begin();

    // Read the raw 12-bit ADC value (0 - 4095)
    int readRaw();

    // Calculate the estimated voltage (0.0 - 3.3V)
    float readVoltage();

    // Get a human-readable string based on pressure thresholds
    String getStatus();
};

#endif