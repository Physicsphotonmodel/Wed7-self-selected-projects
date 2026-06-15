#ifndef HX710_SENSOR_H
#define HX710_SENSOR_H

#include <Arduino.h>

class Hx710Sensor {
private:
    uint8_t _outPin;
    uint8_t _sckPin;
    long _offset;
    
    // PID 參數與狀態變數
    float _kp, _ki, _kd;
    long _integral;
    long _previousError;

public:
    // 1. 無參數建構子：讓 Controller 能夠在成員初始化列表外宣告物件
    Hx710Sensor() : 
        _outPin(0), _sckPin(0), _offset(0), 
        _kp(0), _ki(0), _kd(0), 
        _integral(0), _previousError(0) {}

    // 2. 有參數建構子
    Hx710Sensor(uint8_t outPin, uint8_t sckPin, float kp, float ki, float kd);

    // 硬體初始化
    void begin();
    
    // 底層通訊
    bool isReady();
    long readRaw();
    long readAverage(uint8_t times);
    
    // 校正與數據處理
    void tare(uint8_t times = 10);
    long getRelativeValue();
    
    float updatePID(uint8_t pumpPin);

    void resetPID();
};

#endif