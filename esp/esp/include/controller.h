#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <Arduino.h>

#include "Hx710Sensor.h"   // 確保你的專案內有此檔案
#include "PressureSensor.h" // 確保你的專案內有此檔案

class Controller {
public:
    Controller();

    // 狀態變數
    float pressure_L;          // 左氣壓
    float pressure_R;          // 右氣壓
    float weight_L;            // 左重量感測值
    float weight_R;            // 右重量感測值
    float target_pressure;     // 目標氣壓
    float slope;               // 線性化斜率 (校正用)

    // 初始化與控制
    void begin();
    void setTargetPressure(float base_pressure);
    float mapToReferenceValue(float raw_x);
    
    // 核心動作
    void updateSensorData();   // 更新壓力與重量數據
    void runInflation();
    void runDeflation();

private:
    float current_inflation_speed; // 當前充氣速度係數
    float current_deflation_speed; // 當前放氣速度係數
};

#endif