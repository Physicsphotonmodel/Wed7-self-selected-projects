#include "Arduino.h"

class Valve {
  private:
    uint8_t _pin;

  public:
    Valve(uint8_t pin);

    void begin();
    void open();
    void close();
};