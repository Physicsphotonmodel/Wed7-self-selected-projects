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

const int FSR_THRESHOLD_L = 10; 
const int FSR_THRESHOLD_R = 10; 

const int PWM_IDLE = 100;
const int PWM_MAINTAIN = 245;

const long P_BASELINE = 10; 

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
    STATE_MAINTAIN,
    STATE_DEFLATE
};

SystemState stateL = STATE_IDLE;
SystemState stateR = STATE_IDLE;

unsigned long lastLogTime = 0;

void processSideL(long currentPress, bool isUserPresent) {
    switch (stateL) {
        case STATE_IDLE:
            valveL.close();
            pumpL.setpwm(PWM_IDLE);
            
            if (isUserPresent) {
                stateL = STATE_MAINTAIN;
                ble_log("L IDLE -> MAINTAIN");
            }
            break;

        case STATE_MAINTAIN:
            valveL.close(); 
            pumpL.setpwm(PWM_MAINTAIN);
            
            if (!isUserPresent) {
                stateL = STATE_DEFLATE;
                ble_log("L MAINTAIN -> DEFLATE");
            }
            break;

        case STATE_DEFLATE:
            valveL.open();
            pumpL.setpwm(0);
            
            if (currentPress <= P_BASELINE) {
                stateL = STATE_IDLE;
                ble_log("L DEFLATE -> IDLE");
            } else if (isUserPresent) {
                stateL = STATE_MAINTAIN;
                ble_log("L DEFLATE -> MAINTAIN");
            }
            break;
    }
}

void processSideR(long currentPress, bool isUserPresent) {
    switch (stateR) {
        case STATE_IDLE:
            valveR.close();
            pumpR.setpwm(PWM_IDLE);
            
            if (isUserPresent) {
                stateR = STATE_MAINTAIN;
                ble_log("R IDLE -> MAINTAIN");
            }
            break;

        case STATE_MAINTAIN:
            valveR.close(); 
            pumpR.setpwm(PWM_MAINTAIN);
            
            if (!isUserPresent) {
                stateR = STATE_DEFLATE;
                ble_log("R MAINTAIN -> DEFLATE");
            }
            break;

        case STATE_DEFLATE:
            valveR.open();
            pumpR.setpwm(0);
            
            if (currentPress <= P_BASELINE) {
                stateR = STATE_IDLE;
                ble_log("R DEFLATE -> IDLE");
            } else if (isUserPresent) {
                stateR = STATE_MAINTAIN;
                ble_log("R DEFLATE -> MAINTAIN");
            }
            break;
    }
}

void processLogging(unsigned long currentMillis, long pressValL, long pressValR, int fsrValL, int fsrValR) {
    if (currentMillis - lastLogTime >= 500) {
        String msg = String(pressValL) + "/" + String(pressValR) + 
                     "/" + String(fsrValL) + "/" + String(fsrValR);
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
    pressL.tare();
    pressR.tare();

    stateL = STATE_IDLE;
    stateR = STATE_IDLE;

    ble_log("System Ready");
}

void loop() {
    ble_loop();
    unsigned long currentMillis = millis();
    
    // Centralized sensor reading ensures no redundant blocking
    long pL = pressL.getRelativeValue();
    long pR = pressR.getRelativeValue();
    bool userL = fsrL.isPressed(FSR_THRESHOLD_L);
    bool userR = fsrR.isPressed(FSR_THRESHOLD_R);
    int rawFsrL = fsrL.readRaw();
    int rawFsrR = fsrR.readRaw();
    
    processSideL(pL, userL);
    processSideR(pR, userR);
    processLogging(currentMillis, pL, pR, rawFsrL, rawFsrR);
}