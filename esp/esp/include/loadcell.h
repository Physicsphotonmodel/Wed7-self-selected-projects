#ifndef LOAD_CELL_H
#define LOAD_CELL_H

#include <Arduino.h>
#include "HX711.h"

class LoadCell {
private:
    HX711 scale;
    uint8_t _dtPin;
    uint8_t _sckPin;

public:
    // Constructor: Assign data and clock pins
    LoadCell(uint8_t dtPin, uint8_t sckPin);

    // Initialize the HX711 sensor
    void begin();

    // Tare the scale to zero
    void tare();

    // Set the calibration factor
    void setCalibration(float factor);

    // Read weight (returns average of 'times' readings)
    float getWeight(uint8_t times = 10);
};

#endif