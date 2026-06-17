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

int systemState = 0;

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

    pressL.tare();
    pressR.tare();

    valveL.close();
    valveR.close();

    ble_log("Started synchronous inflation...");
}

void loop() {

    if(systemState == 0){

        if(is_ble_connected){
            float seconds = millis() / 1000;
        
            ble_log("[Hx]L:" + String(analogRead(PIN_HX710_OUT_L)));
            ble_log("[Hx]R:" + String(analogRead(PIN_HX710_OUT_R)));
  
            if(seconds > 1){

                pumpL.setpwm(255);
                pumpR.setpwm(255);
                ble_log("[FSR]L:" + String(analogRead(PIN_FSR_L)));
                ble_log("[FSR]R:" + String(analogRead(PIN_FSR_R)));
                ble_log("[Hx]L:" + String(analogRead(PIN_HX710_OUT_L)));
                ble_log("[Hx]R:" + String(analogRead(PIN_HX710_OUT_R)));
                ble_log("+============================================+");
            
                if (fsrL.isPressed() || fsrR.isPressed()) {

                    pumpL.setpwm(0);
                    pumpR.setpwm(0);
                    valveL.close();
                    valveR.close();

                    // 輸出此時的氣壓值
                    long valL = pressL.getRelativeValue();
                    long valR = pressR.getRelativeValue();
                    
                    ble_log("Critical state reac");
                    ble_log("P L: " + String(valL) + " | P R: " + String(valR));

                    systemState = 1;
                }
            }

        else{
            ble_log("Exit system loop and abort!!");
            ble_log("[FSR]L:" + String(analogRead(PIN_FSR_L)));
            ble_log("[FSR]R:" + String(analogRead(PIN_FSR_R)));
            ble_log("[Hx]L:" + String(analogRead(PIN_HX710_OUT_L)));
            ble_log("[Hx]R:" + String(analogRead(PIN_HX710_OUT_R)));
            ble_log("+============================================+");

            }

        }

    }
}