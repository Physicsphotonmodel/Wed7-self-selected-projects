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
const int PIN_VALVE_L = 13; 
const int PIN_VALVE_R = 27; 
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
// Global State Variables
// ==========================================
int state = 0;              // 流程控制狀態
unsigned long lastTime = 0; // 用於計時

void setup(){
    Serial.begin(115200);
    ble_setup();
    analogWriteResolution(8);
    
    pumpL.begin(); pumpL.off();
    pumpR.begin(); pumpR.off();
    valveL.begin(); valveL.close();
    valveR.begin(); valveR.close();
    fsrL.begin(); fsrR.begin();
    pressL.begin(); pressR.begin();
    
    ble_log("System Initialized");
}


void Ltest_logic() {
     unsigned long now = millis();
     switch(state) {
         case 0:
             valveL.open();
             ble_log("Left open");
             lastTime = now;
             state = 1;
             break;

         case 1:
             if (now - lastTime >= 1000) { 
                 valveL.close();
                 ble_log("Left close");
                 state = 2; 
                 lastTime = now;
             }
            break;
     }
 }

void Rtest_logic() {
     unsigned long now = millis();
     switch(state) {
         case 2:
             valveR.open();
             ble_log("Right open");
             lastTime = now;
            state = 3;
             break;
         case 3:
             if (now - lastTime >= 1000) {
                 valveR.close();
                 ble_log("Right close");
                 lastTime = now;
                 state = 0;
             }
             break;
     }
}

void loop() {
    
    Ltest_logic();
    Rtest_logic();
    
}