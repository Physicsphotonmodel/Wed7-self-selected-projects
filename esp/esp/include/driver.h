#ifndef DRIVER_H
#define DRIVER_H
#include <Arduino.h>

class Driver {
  private:
    uint8_t _pin;

  public:
    Driver(uint8_t pin);

    void begin();

    void on();

    void off();

    void setIntensity(uint8_t percent);
};

#endif