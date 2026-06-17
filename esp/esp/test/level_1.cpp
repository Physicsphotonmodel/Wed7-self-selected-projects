// pumping 15s and hold, most basic case, no ble
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
const int FSR_THRESHOLD_L = 150; // Adjust based on testing
const int FSR_THRESHOLD_R = 300; // Adjust based on testing

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

const unsigned long PUMP_MAX_TIME = 15000;
unsigned long pumpTimerL = 0;
unsigned long pumpTimerR = 0;

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

    // delay(2000); 
    pressL.tare();
    pressR.tare();
}

void loop() {

    if (fsrL.isPressed(FSR_THRESHOLD_L)) {

        if (millis() - pumpTimerL < PUMP_MAX_TIME) {
            valveL.close(); 
            pumpL.on();
        } else {
            pumpL.off();
        }
    } else {
        pumpL.off();
        valveL.open(); 
        pumpTimerL = millis();
    }

    if (fsrR.isPressed(FSR_THRESHOLD_R)) {
        if (millis() - pumpTimerR < PUMP_MAX_TIME) {
            valveR.close();
            pumpR.on();
        } else {
            pumpR.off();
        }
    } else {

        pumpR.off();
        valveR.open();
        pumpTimerR = millis();
    }
}