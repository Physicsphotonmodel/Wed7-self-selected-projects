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
Hx710Sensor pressR(PIN_HX710_OUT_R, PIN_HX710_SCK_R, 0.5, 0.01, 0.2);

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
    valveL.begin(); valveL.close();
    valveR.begin(); valveR.close();
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
        if (pressL.updatePID(PIN_PUMP_L, PIN_VALVE_L)) {
        } 
        
        else {
            valveL.close();
        }
        
        if (millis() - pumpTimerL >= PUMP_MAX_TIME) {
            analogWrite(PIN_PUMP_L, 0);
            valveL.close();
        }

    } else {
        analogWrite(PIN_PUMP_L, 0);
        valveL.open(); 
        pressL.resetPID();
        pumpTimerL = millis();
    }

    if (isHeavyR) {
        if (pressR.updatePID(PIN_PUMP_R, PIN_VALVE_R)) {
        } else {
            valveR.close();
        }
        
        if (millis() - pumpTimerR >= PUMP_MAX_TIME) {
            analogWrite(PIN_PUMP_R, 0);
            valveR.close();
        }
    } else {
        analogWrite(PIN_PUMP_R, 0);
        valveR.open();
        pressR.resetPID();
        pumpTimerR = millis();
    }

    if (millis() - lastLogTime > 500) {
        lastLogTime = millis();
        String logMsg = "[L] Pres: " + String(pressL.getRelativeValue()) + 
                        " | [R] Pres: " + String(pressR.getRelativeValue());
        ble_log(logMsg);
    }
    delay(10); 
}