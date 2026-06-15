#ifndef HX710_SENSOR_H
#define HX710_SENSOR_H

#include <Arduino.h>

class Hx710Sensor {
public:
    Hx710Sensor(uint8_t outPin, uint8_t sckPin);
    void begin();
    bool isReady();
    long readRaw();
    
    // 新增：取多次平均值，過濾雜訊
    long readAverage(uint8_t times = 10);
    
    // 歸零基準線
    void tare(uint8_t times = 20);
    
    // 取得過濾且校正方向後的相對壓力值
    long getRelativeValue();

private:
    uint8_t _outPin;
    uint8_t _sckPin;
    long _offset; 
};

#endif