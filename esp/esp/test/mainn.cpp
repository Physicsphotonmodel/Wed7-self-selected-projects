#include <Arduino.h>
#include "driver.h"
#include "bluetooth.h"
#include "PressureSensor.h"
#include "Hx710Sensor.h"

// ==========================================
// Pin Definitions (ESP32 Safe Pins)
// ==========================================
const int PIN_PUMP_L  = 27;
const int PIN_PUMP_R  = 26; 
const int PIN_VALVE_L = 32; 
const int PIN_VALVE_R = 33; 

const int PIN_FSR_L = 34;
const int PIN_FSR_R = 35;

const int PIN_HX710_OUT_L = 19;
const int PIN_HX710_SCK_L = 18;

const int PIN_HX710_OUT_R = 21;
const int PIN_HX710_SCK_R = 22;

// ==========================================
// Object Instantiation
// ==========================================
Driver pumpL(PIN_PUMP_L);
Driver pumpR(PIN_PUMP_R);
Driver valveL(PIN_VALVE_L);
Driver valveR(PIN_VALVE_R);

PressureSensor fsrL(PIN_FSR_L);
PressureSensor fsrR(PIN_FSR_R);

// 建立物件並帶入初始 PID 參數 (DOUT, SCK, Kp, Ki, Kd)
Hx710Sensor pressL(PIN_HX710_OUT_L, PIN_HX710_SCK_L, 0.5, 0.01, 0.2);
Hx710Sensor pressR(PIN_HX710_OUT_R, PIN_HX710_SCK_R, 0.5, 0.01, 0.2);

// ==========================================
// Constants & Thresholds
// ==========================================
const unsigned long PUMP_MAX_TIME = 15000; // 15 秒安全超時保護
const long MAX_AIR_PRESSURE = 13000;       // PID 目標設定值

// If start pumping then using PID to control, obeying all-yes-all-no principle:
// If the head is detected, the pump start working until one of the conditions is met.
// (1) over 15 seconds have passed, or
// (2) the pressure reaches the target value (13000)


// ==========================================
// State Variables
// ==========================================
bool wasHeavyL = false;
bool isPumpingL = false;
unsigned long pumpTimerL = 0;

bool wasHeavyR = false;
bool isPumpingR = false;
unsigned long pumpTimerR = 0;

unsigned long lastLogTime = 0; 

void setup() {    
    ble_setup();
    ble_log("\n=== Smart Pillow System Init ===");

    pumpL.begin(); pumpL.off();
    pumpR.begin(); pumpR.off();
    valveL.begin(); valveL.off();
    valveR.begin(); valveR.off();

    fsrL.begin();
    fsrR.begin();
    pressL.begin();
    pressR.begin();

    ble_log("Please keep the pillow DEFLATED. Taring sensors in 3 seconds...");
    delay(3000); 

    pressL.tare();
    pressR.tare();

    ble_log("System Ready. Waiting for user to lie down...");
}

void loop() {
    ble_loop(); 

    // 讀取 FSR 與當前相對氣壓
    String statusL = fsrL.getStatus();
    bool isHeavyL = (statusL == "Heavy Press");
    
    String statusR = fsrR.getStatus();
    bool isHeavyR = (statusR == "Heavy Press");

    long airPressL = pressL.getRelativeValue();
    long airPressR = pressR.getRelativeValue();

    // ==========================================
    // Left Side Logic (左側 PID 控制邏輯)
    // ==========================================
    if (isHeavyL && !wasHeavyL) {
        ble_log("[L] Head detected. Valve CLOSED, PID Pumping Start.");
        valveL.on();  // 關閉排氣閥開始鎖氣
        isPumpingL = true;
        pumpTimerL = millis(); 
        pressL.resetPID(); // 清空歷史積分防暴衝
    }
    else if (!isHeavyL && wasHeavyL) {
        ble_log("[L] Head removed. Pump OFF, Valve OPEN.");
        analogWrite(PIN_PUMP_L, 0); // 強制關閉氣泵
        valveL.off();               // 開啟排氣閥洩氣
        isPumpingL = false;
    }

    if (isPumpingL) {
        // 呼叫 PID 運算核心，它會自動調節 PWM 輸出。達標時會關閉馬達並回傳 false
        bool stillRunning = pressL.updatePID(PIN_PUMP_L, MAX_AIR_PRESSURE);
        
        if (!stillRunning) {
            ble_log("[L] PID Target Pressure Reached! Maintaining...");
            isPumpingL = false; // 退出充氣狀態，保持電磁閥關閉以維持壓力
        }

        // 安全超時保護
        if (millis() - pumpTimerL >= PUMP_MAX_TIME) {
            ble_log("[L] 15s Timeout reached. Safety Pump OFF.");
            analogWrite(PIN_PUMP_L, 0);
            isPumpingL = false; 
        }
    }
    wasHeavyL = isHeavyL;

    // ==========================================
    // Right Side Logic (右側 PID 控制邏輯)
    // ==========================================
    if (isHeavyR && !wasHeavyR) {
        ble_log("[R] Head detected. Valve CLOSED, PID Pumping Start.");
        valveR.on();  
        isPumpingR = true;
        pumpTimerR = millis(); 
        pressR.resetPID();
    }
    else if (!isHeavyR && wasHeavyR) {
        ble_log("[R] Head removed. Pump OFF, Valve OPEN.");
        analogWrite(PIN_PUMP_R, 0);
        valveR.off(); 
        isPumpingR = false;
    }

    if (isPumpingR) {
        bool stillRunning = pressR.updatePID(PIN_PUMP_R, MAX_AIR_PRESSURE);
        
        if (!stillRunning) {
            ble_log("[R] PID Target Pressure Reached! Maintaining...");
            isPumpingR = false;
        }

        if (millis() - pumpTimerR >= PUMP_MAX_TIME) {
            ble_log("[R] 15s Timeout reached. Safety Pump OFF.");
            analogWrite(PIN_PUMP_R, 0);
            isPumpingR = false; 
        }
    }
    wasHeavyR = isHeavyR;

    // ==========================================
    // Telemetry & Logging (格式化輸出)
    // ==========================================
    if (millis() - lastLogTime > 500) {
        lastLogTime = millis();
        
        // --- 左側格式化輸出 ---
        // 假設 getRelativeValue() 取得壓力, getPWM() 取得當前值, getTargetPWM() 取得 PID 計算後的預測值
        String logL = "[L] Pressure Value: " + String(airPressL) + 
                      " / PWMcurrent_L: " + String(PIN_PUMP_L) + " /";
        
        // --- 右側格式化輸出 ---
        String logR = "[R] Pressure Value: " + String(airPressR) + 
                      " / PWMcurrent_R: " + String(PIN_PUMP_R) + " /"; 

        // 透過藍牙傳送並顯示在 Serial Monitor
        ble_log(logL);
        ble_log(logR);
        ble_log("--------------------------------------------------"); // 分隔線更易讀
    }

    delay(10); 
}