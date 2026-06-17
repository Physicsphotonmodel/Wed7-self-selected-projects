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
Hx710Sensor pressR(PIN_HX710_OUT_R, PIN_HX710_SCK_R, 0.5, 0.01, 0.2);

const unsigned long PUMP_MAX_TIME = 15000;
unsigned long pumpTimerL = 0;
unsigned long pumpTimerR = 0;

void setup() { 
    Serial.begin(115200);
    // ble_setup();
    analogWriteResolution(8);
    
    pumpL.begin(); pumpL.off();
    pumpR.begin(); pumpR.off();
    valveL.begin(); valveL.close();
    valveR.begin(); valveR.close();
    fsrL.begin(); fsrR.begin();
    pressL.begin(); pressR.begin();

    // delay(2000); 
    pressL.tare();
    pressR.tare();
}

void loop() {
    Serial.println("L:");
    Serial.println(fsrL.readRaw());
    Serial.println("R:");
    Serial.println(fsrR.readRaw());
    Serial.println("==========================");
    // if (fsrL.isPressed()) {

    //     if (millis() - pumpTimerL < PUMP_MAX_TIME) {
    //         valveL.close(); // 關閥保壓
    //         pumpL.on();     // 開啟幫浦
    //     } else {
    //         pumpL.off();    // 超時強制關閉
    //     }
    // } else {
    //     // 沒有壓到，洩氣並重置計時器
    //     pumpL.off();
    //     valveL.open(); 
    //     pumpTimerL = millis();
    // }

    // if (fsrR.isPressed()) {
    //     if (millis() - pumpTimerR < PUMP_MAX_TIME) {
    //         valveR.close();
    //         pumpR.on();
    //     } else {
    //         pumpR.off();
    //     }
    // } else {

    //     pumpR.off();
    //     valveR.open();
    //     pumpTimerR = millis();
    // }
}