#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <Arduino.h>
#include "Hx710Sensor.h"
#include "PressureSensor.h"
#include "pump.h"
#include "valve.h"

// 定義腳位常數 (建議放在這邊或分開的 constants.h)


class Controller {
private:
    // 感測器與硬體物件
    Hx710Sensor sensorL;
    Hx710Sensor sensorR;
    Pump pumpL;
    Pump pumpR;
    Valve valveL;
    Valve valveR;
    PressureSensor pressL;
    PressureSensor pressR;

    float target_pressure;

public:
    // 建構子：使用成員初始化列表
    Controller();

    // 初始化硬體
    void begin();

    // 設定目標壓力
    void setTargetPressure(float base_pressure);

    // PID 控制核心
    void PIDcontrol();

    // 單側充氣邏輯
    void singleSideInflation();

    // 過充保護邏輯
    void overshoot_deal();
};

#endif