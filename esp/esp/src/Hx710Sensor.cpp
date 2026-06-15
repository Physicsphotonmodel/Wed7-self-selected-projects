#include "Hx710Sensor.h"

float MAX_VAL = 500000;


// PID修正至誤差小於１百分比以內。

Hx710Sensor::Hx710Sensor(uint8_t outPin, uint8_t sckPin, float kp, float ki, float kd) {
    _outPin = outPin;
    _sckPin = sckPin;
    _offset = 0;
    
    // 初始化 PID 參數與狀態
    _kp = kp;
    _ki = ki;
    _kd = kd;
    _integral = 0;
    _previousError = 0;
}

void Hx710Sensor::begin() {
    pinMode(_sckPin, OUTPUT);
    pinMode(_outPin, INPUT);
    digitalWrite(_sckPin, LOW); 
}

bool Hx710Sensor::isReady() {
    return digitalRead(_outPin) == LOW;
}

long Hx710Sensor::readRaw() {
    while (!isReady()) {
        yield(); 
    }

    long count = 0;
    
    for (int i = 0; i < 24; i++) {
        digitalWrite(_sckPin, HIGH);
        delayMicroseconds(1);
        count = count << 1;
        digitalWrite(_sckPin, LOW);
        delayMicroseconds(1);
        if (digitalRead(_outPin)) {
            count++;
        }
    }

    // 發送第 25 個時脈脈衝 (設定 HX710 工作模式為 10Hz 輸入)
    digitalWrite(_sckPin, HIGH);
    delayMicroseconds(1);
    digitalWrite(_sckPin, LOW);
    delayMicroseconds(1);

    // 處理二進位補數：將 24-bit 有號整數擴展為 32-bit 有號整數 (解決負數問題)
    if (count & 0x800000) {
        count |= 0xFF000000; 
    }
    
    return count;
}

long Hx710Sensor::readAverage(uint8_t times) {
    if (times == 0) times = 1;
    long sum = 0;
    for (int i = 0; i < times; i++) {
        sum += readRaw();
    }
    return sum / times;
}

void Hx710Sensor::tare(uint8_t times) {
    readRaw(); readRaw();
    // 讀取當前平均值作為歸零基準線
    _offset = readAverage(times);
}

long Hx710Sensor::getRelativeValue() {
    // 1. 獲取多次濾波後的原始讀值
    long currentRaw = readAverage(5); 
    long diff = currentRaw - _offset;

    diff = map(diff, 0, MAX_VAL, 0, 100);
    return diff;
}


float Hx710Sensor::updatePID(uint8_t pumpPin) {

    long current = getRelativeValue();
    long error = 100 - current;
    
    // 積分項：累積誤差，並加入防積分飽和機制
    _integral += error;
    _integral = constrain(_integral, -10000, 10000); 

    // 微分項：誤差變化率
    long derivative = error - _previousError;
    _previousError = error;

    // PID 計算
    long output = (long)(_kp * error + _ki * _integral + _kd * derivative);
    output = constrain(output, 0, 255); 

    return output;

}

void Hx710Sensor::resetPID() {
    _integral = 0;
    _previousError = 0;
}
