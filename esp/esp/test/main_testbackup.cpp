#include <Arduino.h>
#include "pump.h"
#include "valve.h"
#include "bluetooth.h"
#include "PressureSensor.h"
#include "Hx710Sensor.h"

// ==========================================
// Pin Definitions & Instantiation
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

Pump pumpL(PIN_PUMP_L);
Pump pumpR(PIN_PUMP_R);
Valve valveL(PIN_VALVE_L);
Valve valveR(PIN_VALVE_R);

// ==========================================
// 狀態機變數
// ==========================================
enum State { INIT, INFLATING, FINISHING, IDLE };
State currentState = INIT;
unsigned long stateStartTime = 0;

void testInflation() {
    unsigned long currentMillis = millis();

    switch (currentState) {
        case INIT:
            ble_log(">>> initial 0.5s...");
            valveL.close();
            valveR.close();
            pumpL.setpwm(255);
            pumpR.setpwm(255);
            stateStartTime = currentMillis;
            currentState = INFLATING;
            break;

        case INFLATING:
            if (currentMillis - stateStartTime >= 3500) {
                int val = analogRead(PIN_HX710_OUT_L);
                ble_log(String(val));
                valveL.open();
                pumpL.setpwm(0);
                pumpR.setpwm(0);
                stateStartTime = currentMillis;
                currentState = FINISHING;
            }
            break;

        case FINISHING:
            if (currentMillis - stateStartTime >= 500) {
                // 回到初始狀態，開始下一次循環
                currentState = INIT;
            }
            break;
    }
}

void setup() {
    ble_setup();
    pumpL.begin();
    pumpR.begin();
    valveL.begin();
    valveR.begin();
    currentState = INIT;
}

void loop() {
    ble_loop();
    // 呼叫狀態機，不再使用 delay
    testInflation();
}