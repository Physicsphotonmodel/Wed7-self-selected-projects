#ifndef PUMP_H
#define PUMP_H
#include <Arduino.h>

class Pump {
  private:
    uint8_t _pin;

  public:
    Pump(uint8_t pin);

    void begin();

    void on();

    void off();

    void setpwm(int pwm_value){
    };

  };

#endif