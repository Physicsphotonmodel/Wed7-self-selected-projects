#ifndef HX710_SENSOR_H
#define HX710_SENSOR_H

#include <Arduino.h>

class Hx710Sensor {
public:
    // 建構子：加入了 PID 參數的初始化設定 (預設給予你測試的參數 Kp=0.5, Ki=0.01, Kd=0.2)
    Hx710Sensor(uint8_t outPin, uint8_t sckPin, float kp = 0.5, float ki = 0.01, float kd = 0.2);
    
    void begin();
    bool isReady();
    long readRaw();
    
    // 讀取多次原始值，濾除雜訊
    long readAverage(uint8_t times = 10);
    
    // 校正零點基準線
    void tare(uint8_t times = 20);
    
    // 獲取濾波、校正、並調整極性後的相對壓力值
    long getRelativeValue();

    /**
     * @brief 執行單次 PID 氣壓充氣控制
     * @param pumpPin  該側氣泵的 PWM 控制接腳
     * @param target   目標氣壓值 (例如 13000)
     * @return true 代表仍在充氣中；false 代表已達標停機或氣壓充足
     */

    bool updatePID(uint8_t pumpPin, long target);

    // 允許在程式執行中，手動重置該側的 PID 狀態（例如人頭離開時需清除積分）
    void resetPID();

    // 允許動態修改該側的 PID 參數
    void setTunings(float kp, float ki, float kd);

private:
    // 硬體接腳與基準值
    uint8_t _outPin;
    uint8_t _sckPin;
    long _offset; 

    float _kp;
    float _ki;
    float _kd;
    long _integral;
    long _previousError;
};

#endif // HX710_SENSOR_H