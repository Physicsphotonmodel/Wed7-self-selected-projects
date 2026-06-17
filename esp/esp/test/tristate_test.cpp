#include <Arduino.h>
#include "pump.h"
#include "valve.h"
#include "bluetooth.h"
#include "PressureSensor.h"
#include "Hx710Sensor.h"

// tristate test without bluetooth

// ==========================================
// Pin Definitions
// ==========================================
const int PIN_PUMP_L = 17;
const int PIN_PUMP_R = 16; 
const int PIN_VALVE_L = 32; 
const int PIN_VALVE_R = 13; 
const int PIN_FSR_L = 34;
const int PIN_FSR_R = 35;
const int PIN_HX710_OUT_L = 4;
const int PIN_HX710_SCK_L = 18;
const int PIN_HX710_OUT_R = 25;
const int PIN_HX710_SCK_R = 22;

// ==========================================
// Object Instantiation
// ==========================================
Pump pumpL(PIN_PUMP_L);
Pump pumpR(PIN_PUMP_R);
Valve valveL(PIN_VALVE_L);
Valve valveR(PIN_VALVE_R);
PressureSensor fsrL(PIN_FSR_L);
PressureSensor fsrR(PIN_FSR_R);
Hx710Sensor pressL(PIN_HX710_OUT_L, PIN_HX710_SCK_L, 0.5, 0, 0);
Hx710Sensor pressR(PIN_HX710_OUT_R, PIN_HX710_SCK_R, 0.5, 0, 0);


enum State { INIT, INFLATING, FINISHING };
State currentState = INIT;
unsigned long stateStartTime = 0;
unsigned long currentMillis = 0;

// Log 限速器變數
unsigned long lastLogTime = 0;

// ==========================================
// Main Functions
// ==========================================
void testInflation() {
    currentMillis = millis();

    if (currentMillis - lastLogTime >= 500) {
        // 正確讀取 HX710 的數位數值
        long valL = pressL.readRaw(); 
        long valR = pressR.readRaw(); 
    
        ble_log("L:" + String(valL) + " R:" + String(valR));
        lastLogTime = currentMillis;
    }

    switch (currentState) {
        case INIT:
            if (currentMillis - stateStartTime > 500) {
                currentState = INFLATING;
                stateStartTime = currentMillis; // 修正：必須更新為當下時間
                
                // 進入打氣狀態的瞬間，執行硬體動作
                valveL.close(); valveR.close();
                pumpL.setpwm(255); pumpR.setpwm(255);
            }
            break;

        case INFLATING:
            if (currentMillis - stateStartTime > 3500) { // 打氣稍微設長一點 (3.5秒) 比較看得出變化
                currentState = FINISHING;
                stateStartTime = currentMillis; // 修正：必須更新為當下時間
                
                // 進入結束狀態的瞬間，執行硬體動作
                valveL.open(); valveR.open();
                pumpL.setpwm(0); pumpR.setpwm(0);
            }
            break;

        case FINISHING:
            if (currentMillis - stateStartTime > 2000) { // 放氣給 2 秒
                currentState = INIT;
                stateStartTime = currentMillis; // 修正：必須更新為當下時間
            }
            break;
    }
}

void setup() {
    Serial.begin(115200);
    ble_setup();
    analogWriteResolution(8); 
    
    pumpL.begin(); pumpL.off();
    pumpR.begin(); pumpR.off();
    valveL.begin(); valveL.open(); 
    valveR.begin(); valveR.open();
    fsrL.begin(); fsrR.begin();
    
    // 初始化氣壓計
    pressL.begin(); pressR.begin();

    currentState = INIT;
    stateStartTime = millis();
    ble_log("System Ready - Test Started");
}

void loop() {
    testInflation();
}