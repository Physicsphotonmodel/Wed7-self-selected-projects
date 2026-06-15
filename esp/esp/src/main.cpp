#include <Arduino.h>
#include "bluetooth.h"

// 腳位定義


const int PIN_PUMP_L = 27;
const int PIN_PUMP_R = 26;
const int PIN_VALVE_L = 32;
const int PIN_VALVE_R = 33;
const int PIN_FSR_L = 34;
const int PIN_FSR_R = 35;


const int PIN_HX710_OUT_L = 19;
const int PIN_HX710_SCK_L = 18;
const int PIN_HX710_OUT_R = 21;
const int PIN_HX710_SCK_R = 22;


const int FSR_THRESHOLD = 500;
const int PWM_SPEED = 50;

// 狀態機變數
unsigned long lastStateTime = 0;
int state = 0; // 0: 測試序列, 1: 壓力感測監控

void setup() {
    Serial.begin(115200);
    ble_setup();
    Serial.println("System Ready.");
}

void loop() {
    unsigned long currentMillis = millis();

    // 1. 壓力感測器即時監控 (永遠優先執行)
    if (analogRead(PIN_FSR_L) > FSR_THRESHOLD) analogWrite(PIN_PUMP_L, PWM_SPEED);
    else analogWrite(PIN_PUMP_L, 0);

    if (analogRead(PIN_FSR_R) > FSR_THRESHOLD) analogWrite(PIN_PUMP_R, PWM_SPEED);
    else analogWrite(PIN_PUMP_R, 0);

    // 2. 使用狀態機取代 delay (每 5 秒切換一個動作)
    if (currentMillis - lastStateTime >= 5000) {
        lastStateTime = currentMillis;
        
        // 關閉所有輸出
        analogWrite(PIN_PUMP_L, 0); analogWrite(PIN_PUMP_R, 0);
        analogWrite(PIN_VALVE_L, 0); analogWrite(PIN_VALVE_R, 0);

        switch(state) {
            case 0: ble_log("Test: Pump L"); analogWrite(PIN_PUMP_L, PWM_SPEED); analogWrite(PIN_VALVE_L, PWM_SPEED);break;
            case 1: ble_log("Test: Pump R"); analogWrite(PIN_PUMP_R, PWM_SPEED); analogWrite(PIN_VALVE_R, PWM_SPEED);break;
            default: state = -1; break;
        }

        state = (state + 1) % 2; // 循環切換狀態
    }
}
