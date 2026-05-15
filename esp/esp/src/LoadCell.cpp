#include "loadcell.h"

LoadCell::LoadCell(uint8_t dtPin, uint8_t sckPin) {
    _dtPin = dtPin;
    _sckPin = sckPin;
}

void LoadCell::begin() {
    scale.begin(_dtPin, _sckPin);
}

void LoadCell::tare() {
    scale.tare();
}

void LoadCell::setCalibration(float factor) {
    scale.set_scale(factor);
}

float LoadCell::getWeight(uint8_t times) {
    return scale.get_units(times);
}