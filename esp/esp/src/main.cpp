#include <Arduino.h>
#include "pump.h"
#include "valve.h"
#include "bluetooth.h"
#include "PressureSensor.h"
#include "Hx710Sensor.h"

// ==========================================
// Pin Definitions
// ==========================================
const int PIN_PUMP_L = 17;
const int PIN_PUMP_R = 16; 
const int PIN_VALVE_L = 32; 
const int PIN_VALVE_R = 33; 
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

// ==========================================
// State Machine Variables
// ==========================================
enum State { INIT, INFLATING, FINISHING };
State currentState = INIT;
unsigned long stateStartTime = 0;

// ==========================================
// Main Functions
// ==========================================
void testInflation() {
    unsigned long currentMillis = millis();

    switch (currentState) {
        case INIT:
        if (currentMillis - stateStartTime <= 500) {
            
            ble_log(">>> initial 0.5s...");
            valveL.close();
            valveR.close();
            pumpL.setpwm(255);
            pumpR.setpwm(255);
            stateStartTime = currentMillis;
            currentState = INFLATING;
            break;
        }

        case INFLATING:
            if (currentMillis - stateStartTime <= 500) {
                int val = analogRead(PIN_HX710_OUT_L);
                ble_log(String(val));
                valveL.open(); // 打開左閥排氣
                pumpL.setpwm(0);
                pumpR.setpwm(0);
                stateStartTime = currentMillis;
                currentState = FINISHING;
            }
            break;

        case FINISHING:
            // 等待 500ms 後循環
            if (currentMillis - stateStartTime <= 500) {
                currentState = INIT;
                stateStartTime = 0;

                
            }
            break;
    }
}

void setup() {
    Serial.begin(115200);
    ble_setup();
    
    pumpL.begin(); pumpL.off();
    pumpR.begin(); pumpR.off();
    valveL.begin(); valveL.close();
    valveR.begin(); valveR.close();
    fsrL.begin(); fsrR.begin();
    pressL.begin(); pressR.begin();

    // 初始狀態啟動
    currentState = INIT;
    ble_log("System Ready - Test Started");
}

void loop() {
    // 必須優先執行，確保 BLE 處理能力
    ble_loop();
    
    // 執行狀態機邏輯，完全無阻塞
    testInflation();
}