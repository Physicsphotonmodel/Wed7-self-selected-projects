#include "Arduino.h"

class Valve {
  
private:
    uint8_t _pin;
  
  
public:

    Valve(uint8_t pin){
        _pin = pin;
    }

    void begin(){
        pinMode(_pin, OUTPUT);
        close(); // 預設為關閉狀態
    };

    void open(){
        digitalWrite(_pin, LOW);
    };

    void close(){
        digitalWrite(_pin, HIGH);
    };
};