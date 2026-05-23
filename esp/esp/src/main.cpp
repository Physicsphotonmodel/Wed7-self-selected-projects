#include <Arduino.h>
#include "driver.h"
#include "bluetooth.h"
#include "PressureSensor.h"

// ==========================================
// Pin Definitions (ESP32 Safe Pins)
// ==========================================
const int PIN_PUMP_L  = 27;
const int PIN_PUMP_R  = 14;
const int PIN_VALVE_L = 32; 
const int PIN_VALVE_R = 33; 

const int PIN_FSR_L = 34;
const int PIN_FSR_R = 35;

// ==========================================
// Object Instantiation
// ==========================================
Driver pumpL(PIN_PUMP_L);
// Driver pumpR(PIN_PUMP_R);
Driver valveL(PIN_VALVE_L);
// Driver valveR(PIN_VALVE_R);

PressureSensor fsrL(PIN_FSR_L);
// PressureSensor fsrR(PIN_FSR_R);

const unsigned long PUMP_MAX_TIME = 15000; // 15 seconds

bool wasHeavyL = false;
bool isPumpingL = false;
unsigned long pumpTimerL = 0;

// bool wasHeavyR = false;
// bool isPumpingR = false;
// unsigned long pumpTimerR = 0;

void setup() {
    Serial.begin(115200);
    while (!Serial) { delay(10); }
    
    ble_setup();
    
    ble_log("\n=== Smart Pillow System Init ===");

    pumpL.begin();
    // pumpR.begin();
    valveL.begin();
    // valveR.begin();

    fsrL.begin();
    // fsrR.begin();

    ble_log("System Ready. Waiting for pressure...");
}

void loop() {
    // ble_loop();

    String statusL = fsrL.getStatus();
    // String statusR = fsrR.getStatus();

    bool isHeavyL = (statusL == "Heavy Press");
    // bool isHeavyR = (statusR == "Heavy Press");

    if (isHeavyL && !wasHeavyL) {
        ble_log("[L] Head detected");
        valveL.on();  
        pumpL.on();   
        isPumpingL = true;
        pumpTimerL = millis(); 
    } 
    else if (!isHeavyL && wasHeavyL) {
        ble_log("[L] Head removed");
        pumpL.off();
        valveL.off(); 
        isPumpingL = false;
    }

    if (isPumpingL && (millis() - pumpTimerL >= PUMP_MAX_TIME)) {
        ble_log("[L] 15s reached");
        pumpL.off();
        isPumpingL = false; 
    }

    // if (isHeavyR && !wasHeavyR) {
    //     ble_log("[R] Head detected");
    //     valveR.on();  
    //     pumpR.on();   
    //     isPumpingR = true;
    //     pumpTimerR = millis(); 
    // } 
    // else if (!isHeavyR && wasHeavyR) {
    //     ble_log("[R] Head removed");
    //     pumpR.off();
    //     valveR.off(); 
    //     isPumpingR = false;
    // }

    // if (isPumpingR && (millis() - pumpTimerR >= PUMP_MAX_TIME)) {
    //     ble_log("[R] 15s reached");
    //     pumpR.off();
    //     isPumpingR = false; 
    // }

    wasHeavyL = isHeavyL;
    // wasHeavyR = isHeavyR;

    delay(50); 
}