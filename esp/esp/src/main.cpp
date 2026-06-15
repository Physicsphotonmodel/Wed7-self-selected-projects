#include <Arduino.h>
#include "driver.h"
#include "bluetooth.h"
#include "PressureSensor.h"
#include "Hx710Sensor.h"

// ==========================================
// Pin Definitions (ESP32 Safe Pins)
// ==========================================
// 氣泵與電磁閥
const int PIN_PUMP_L  = 27;
const int PIN_PUMP_R  = 26; // ? 取代原本危險的 14，換成絕對安全的 26
const int PIN_VALVE_L = 32; 
const int PIN_VALVE_R = 33; 

// FSR 壓力感測 (維持原本完美的 Input-only 腳位)
const int PIN_FSR_L = 34;
const int PIN_FSR_R = 35;

// HX710 氣壓計腳位 (左)
const int PIN_HX710_OUT_L = 19;
const int PIN_HX710_SCK_L = 18;

// HX710 氣壓計腳位 (右)
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

Hx710Sensor pressL(PIN_HX710_OUT_L, PIN_HX710_SCK_L);
Hx710Sensor pressR(PIN_HX710_OUT_R, PIN_HX710_SCK_R);

// ==========================================
// Constants & Thresholds
// ==========================================
const unsigned long PUMP_MAX_TIME = 15000; // 最大超時保護 15 秒
const long MAX_AIR_PRESSURE = 13000;       // 你測試成功的 100% 飽滿度氣壓值

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

    // 初始化致動器並確保關閉
    pumpL.begin(); pumpL.off();
    pumpR.begin(); pumpR.off();
    valveL.begin(); valveL.off();
    valveR.begin(); valveR.off();

    // 初始化感測器
    fsrL.begin();
    fsrR.begin();
    pressL.begin();
    pressR.begin();

    ble_log("Please keep the pillow DEFLATED. Taring sensors in 3 seconds...");
    delay(3000); // 給予硬體穩定時間

    // 氣壓計開機歸零 (非常重要)
    pressL.tare();
    pressR.tare();

    ble_log("System Ready. Waiting for user to lie down...");
}

void loop() {
    ble_loop(); // 維持藍牙連線

    // 讀取 FSR 狀態
    String statusL = fsrL.getStatus();
    bool isHeavyL = (statusL == "Heavy Press");
    
    String statusR = fsrR.getStatus();
    bool isHeavyR = (statusR == "Heavy Press");

    // 全速讀取管路即時氣壓
    long airPressL = pressL.getRelativeValue();
    long airPressR = pressR.getRelativeValue();

    // ==========================================
    // Left Side Logic (左側控制邏輯)
    // ==========================================
    // 1. 觸發打氣：剛偵測到頭部壓下
    if (isHeavyL && !wasHeavyL) {
        ble_log("[L] Head detected. Valve CLOSED, Pump ON.");
        valveL.on();  // 關閉排氣閥 (封閉管路)
        pumpL.on();   // 啟動氣泵
        isPumpingL = true;
        pumpTimerL = millis(); 
    }
    // 2. 觸發排氣：頭部離開
    else if (!isHeavyL && wasHeavyL) {
        ble_log("[L] Head removed. Pump OFF, Valve OPEN.");
        pumpL.off();
        valveL.off(); // 打開排氣閥 (放氣)
        isPumpingL = false;
    }

    // 3. 安全停止 A：超過設定的目標氣壓 (13000)
    if (isPumpingL && (airPressL >= MAX_AIR_PRESSURE)) {
        ble_log("[L] Target Pressure Reached! Pump Auto-Stopped.");
        pumpL.off();
        isPumpingL = false; // 氣壓達標，停止打氣，但電磁閥保持關閉(維持飽滿)
    }

    // 4. 安全停止 B：打氣超時防護 (15秒)
    if (isPumpingL && (millis() - pumpTimerL >= PUMP_MAX_TIME)) {
        ble_log("[L] 15s Timeout reached. Safety Pump OFF.");
        pumpL.off();
        isPumpingL = false; 
    }
    
    wasHeavyL = isHeavyL;

    // ==========================================
    // Right Side Logic (右側控制邏輯)
    // ==========================================
    // 1. 觸發打氣：剛偵測到頭部壓下
    if (isHeavyR && !wasHeavyR) {
        ble_log("[R] Head detected. Valve CLOSED, Pump ON.");
        valveR.on();  
        pumpR.on();   
        isPumpingR = true;
        pumpTimerR = millis(); 
    }
    // 2. 觸發排氣：頭部離開
    else if (!isHeavyR && wasHeavyR) {
        ble_log("[R] Head removed. Pump OFF, Valve OPEN.");
        pumpR.off();
        valveR.off(); 
        isPumpingR = false;
    }

    // 3. 安全停止 A：超過設定的目標氣壓 (13000)
    if (isPumpingR && (airPressR >= MAX_AIR_PRESSURE)) {
        ble_log("[R] Target Pressure Reached! Pump Auto-Stopped.");
        pumpR.off();
        isPumpingR = false; 
    }

    // 4. 安全停止 B：打氣超時防護 (15秒)
    if (isPumpingR && (millis() - pumpTimerR >= PUMP_MAX_TIME)) {
        ble_log("[R] 15s Timeout reached. Safety Pump OFF.");
        pumpR.off();
        isPumpingR = false; 
    }

    wasHeavyR = isHeavyR;

    // ==========================================
    // Telemetry & Logging (遙測回報)
    // ==========================================
    if (millis() - lastLogTime > 500) {
        lastLogTime = millis();
        // 只有在打氣或有人躺著的時候才頻繁印 Log，避免平常洗版
        if (isHeavyL || isHeavyR || isPumpingL || isPumpingR) {
            String logMsg = "Press [L]:" + String(airPressL) + 
                            " [R]:" + String(airPressR);
            ble_log(logMsg);
        }
    }

    delay(10); // 系統微小喘息，確保藍牙穩定
}