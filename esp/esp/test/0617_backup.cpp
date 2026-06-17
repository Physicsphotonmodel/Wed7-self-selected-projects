#include <Arduino.h>
#include "pump.h"
#include "valve.h"
#include "bluetooth.h"
#include "PressureSensor.h"
#include "Hx710Sensor.h"

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

const long P_MAX = 3350000; 
const long P1 = P_MAX * 0.85; 
const long P2 = P_MAX * 0.95; 
//need P3?

// Tolerance to prevent pump/valve chattering around target pressure, can adjust?
const long P_TOL = 30000; 

Pump pumpL(PIN_PUMP_L);
Pump pumpR(PIN_PUMP_R);
Valve valveL(PIN_VALVE_L);
Valve valveR(PIN_VALVE_R);
PressureSensor fsrL(PIN_FSR_L);
PressureSensor fsrR(PIN_FSR_R);
Hx710Sensor pressL(PIN_HX710_OUT_L, PIN_HX710_SCK_L, 0.5, 0, 0);
Hx710Sensor pressR(PIN_HX710_OUT_R, PIN_HX710_SCK_R, 0.5, 0, 0);

enum SystemState {
    STATE_IDLE,
    STATE_INFLATE,
    STATE_HOLD,
    STATE_DEFLATE
};

SystemState stateL = STATE_IDLE;
SystemState stateR = STATE_IDLE;

unsigned long lastLogTime = 0;

void processSideL() {
    long currentPress = pressL.getRelativeValue();
    bool isUserPresent = fsrL.isPressed();

    switch (stateL) {
        case STATE_IDLE:
            if (isUserPresent) {
                stateL = STATE_INFLATE;
                ble_log("L S0->S1");
            } else if (currentPress < (P1 - P_TOL)) {
                valveL.close();
                pumpL.setpwm(255);
            } else if (currentPress > (P1 + P_TOL)) {
                valveL.open();
                pumpL.setpwm(0);
            } else {
                valveL.close();
                pumpL.setpwm(0);
            }
            break;

        case STATE_INFLATE:
            valveL.close();
            pumpL.setpwm(255);
            if (!isUserPresent) {
                stateL = STATE_DEFLATE;
                ble_log("L S1->S3");
            } else if (currentPress >= P_MAX) {
                stateL = STATE_HOLD;
                ble_log("L S1->S2");
            }
            break;

        case STATE_HOLD:
            valveL.close();
            pumpL.setpwm(0);
            if (!isUserPresent) {
                stateL = STATE_DEFLATE;
                ble_log("L S2->S3");
            } else if (currentPress <= P2) {
                stateL = STATE_INFLATE;
                ble_log("L S2->S1");
            }
            break;

        case STATE_DEFLATE:
            valveL.open();
            pumpL.setpwm(0);
            if (currentPress <= P1) {
                stateL = STATE_IDLE;
                ble_log("L S3->S0");
            } else if (isUserPresent) {
                stateL = STATE_INFLATE;
                ble_log("L S3->S1");
            }
            break;
    }
}

void processSideR() {
    long currentPress = pressR.getRelativeValue();
    bool isUserPresent = fsrR.isPressed();

    switch (stateR) {
        case STATE_IDLE:
            if (isUserPresent) {
                stateR = STATE_INFLATE;
                ble_log("R S0->S1");
            } else if (currentPress < (P1 - P_TOL)) {
                valveR.close();
                pumpR.setpwm(255);
            } else if (currentPress > (P1 + P_TOL)) {
                valveR.open();
                pumpR.setpwm(0);
            } else {
                valveR.close();
                pumpR.setpwm(0);
            }
            break;

        case STATE_INFLATE:
            valveR.close();
            pumpR.setpwm(255);
            if (!isUserPresent) {
                stateR = STATE_DEFLATE;
                ble_log("R S1->S3");
            } else if (currentPress >= P_MAX) {
                stateR = STATE_HOLD;
                ble_log("R S1->S2");
            }
            break;

        case STATE_HOLD:
            valveR.close();
            pumpR.setpwm(0);
            if (!isUserPresent) {
                stateR = STATE_DEFLATE;
                ble_log("R S2->S3");
            } else if (currentPress <= P2) {
                stateR = STATE_INFLATE;
                ble_log("R S2->S1");
            }
            break;

        case STATE_DEFLATE:
            valveR.open();
            pumpR.setpwm(0);
            if (currentPress <= P1) {
                stateR = STATE_IDLE;
                ble_log("R S3->S0");
            } else if (isUserPresent) {
                stateR = STATE_INFLATE;
                ble_log("R S3->S1");
            }
            break;
    }
}

void processLogging(unsigned long currentMillis) {
    if (currentMillis - lastLogTime >= 500) {
        long pressValL = pressL.getRelativeValue();
        long pressValR = pressR.getRelativeValue();
        int fsrValL = fsrL.readRaw();
        int fsrValR = fsrR.readRaw();

        String msg = "PL:" + String(pressValL) + " PR:" + String(pressValR) + 
                     " FL:" + String(fsrValL) + " FR:" + String(fsrValR);
        ble_log(msg);

        lastLogTime = currentMillis;
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
    
    pressL.begin(); pressR.begin(); 
    delay(2000); 
    pressL.tare();
    pressR.tare();

    stateL = STATE_IDLE;
    stateR = STATE_IDLE;

    ble_log("System Ready");
}

void loop() {
    ble_loop();
    unsigned long currentMillis = millis();
    
    processSideL();
    processSideR();
    processLogging(currentMillis);
}