#ifndef HX710_SENSOR_H
#define HX710_SENSOR_H

#include <Arduino.h>

class Hx710Sensor {
private:
    uint8_t _outPin;
    uint8_t _sckPin;
    long _offset;
    
    float _kp, _ki, _kd;
    long _integral;
    long _previousError;

public:
    Hx710Sensor() : 
        _outPin(0), _sckPin(0), _offset(0), 
        _kp(0), _ki(0), _kd(0), 
        _integral(0), _previousError(0) {}

    Hx710Sensor(uint8_t outPin, uint8_t sckPin, float kp, float ki, float kd);

    void begin();
    
    bool isReady();
    long readRaw();
    long readAverage(uint8_t times);
    
    void tare(uint8_t times = 10);
    long getRelativeValue();
    
    bool updatePID(uint8_t pumpPin, uint8_t valvePin);

    int updatePID_cont(void);

    void resetPID();

    void readAndPrint();
};

#endif