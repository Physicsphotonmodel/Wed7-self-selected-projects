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
const int PIN_HX710_OUT_L = 19;
const int PIN_HX710_SCK_L = 18;
const int PIN_HX710_OUT_R = 21;
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
// Hx710Sensor pressL(PIN_HX710_OUT_L, PIN_HX710_SCK_L, 0.5, 0.01, 0.2);
// Hx710Sensor pressR(PIN_HX710_OUT_R, PIN_HX710_SCK_R, 0.5, 0.01, 0.2);
Hx710Sensor pressL(PIN_HX710_OUT_L, PIN_HX710_SCK_L, 0, 0, 0);
Hx710Sensor pressR(PIN_HX710_OUT_R, PIN_HX710_SCK_R, 0, 0, 0);

const unsigned long PUMP_MAX_TIME = 15000;

bool wasHeavyL = false, isPumpingL = false;
unsigned long pumpTimerL = 0;
bool wasHeavyR = false, isPumpingR = false;
unsigned long pumpTimerR = 0;
unsigned long lastLogTime = 0; 

void setup() { 
    Serial.begin(115200);
    ble_setup();
    analogWriteResolution(8);
    
    pumpL.begin(); pumpL.off();
    pumpR.begin(); pumpR.off();
    valveL.begin(); valveL.open();
    valveR.begin(); valveR.open();
    fsrL.begin(); fsrR.begin();
    pressL.begin(); pressR.begin();

    delay(2000); 
    pressL.tare();
    pressR.tare();
}


void loop() {
    ble_loop();

    bool isHeavyL = fsrL.isPressed();
    bool isHeavyR = fsrR.isPressed();

    if (isHeavyL) {

        int pwmL = pressL.updatePID_cont(); 
        
        if (pwmL > 0) {
            valveL.close();
            pumpL.setpwm(pwmL);
        } else if (pwmL < 0) {

            pumpL.setpwm(0);
            valveL.open();
        } else {
            // 已達標，鎖住氣壓
            valveL.close();
            pumpL.setpwm(0);
        }
    } 
    
    else {
        pumpL.setpwm(0);
        valveL.open();
        pressL.resetPID();
    }

    // --- 右側連續控制 ---
    if (isHeavyR) {
        int pwmR = pressR.updatePID_cont();
        
        if (pwmR > 0) {
            valveR.close();
            pumpR.setpwm(pwmR);
        } else if (pwmR < 0) {
            pumpR.setpwm(0);
            valveR.open();
        } else {
            valveR.close();
            pumpR.setpwm(0);
        }
    } else {
        pumpR.setpwm(0);
        valveR.open();
        pressR.resetPID();
    }

    if (millis() - lastLogTime > 500) {
        lastLogTime = millis();
        ble_log("[L] Pres: " + String(pressL.getRelativeValue()) + " | [R] Pres: " + String(pressR.getRelativeValue()));
    }
}