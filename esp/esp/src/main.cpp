#include "Arduino.h"
#include "pump.h"       
#include "valve.h"
#include "PressureSensor.h"
#include "Hx710Sensor.h"   
#include "bluetooth.h"

const int PIN_PUMP_L = 17;
const int PIN_VALVE_L = 32;
const int PIN_FSR_L = 34;          
const int PIN_HX710_OUT_L = 4;     
const int PIN_HX710_SCK_L = 18;    

Pump pumpL(PIN_PUMP_L);
Valve valveL(PIN_VALVE_L);
PressureSensor fsrL(PIN_FSR_L);

Hx710Sensor airPressL(PIN_HX710_OUT_L, PIN_HX710_SCK_L, 1.0, 0, 0);

unsigned long stateTimer = 0;
int testState = 0;

void testInflation() {
    unsigned long currentMillis = millis();

    switch (testState) {
        case 0:
            ble_log("S0:V_CLS, P_ON"); 
            valveL.close();      
            pumpL.setpwm(255); 
            stateTimer = millis();
            testState = 1;
            break;

        case 1:
            if (currentMillis - stateTimer >= 3500) {
                int fsrVal = fsrL.readRaw(); 
                long airVal = airPressL.getRelativeValue(); 

                String msg = "F:" + String(fsrVal) + " A:" + String(airVal);
                ble_log(msg);
                
                ble_log("S1:V_OPN, P_OFF");
                valveL.open();
                pumpL.setpwm(0);    
                
                stateTimer = millis(); 
                testState = 2;
            }
            break;

        case 2:
            if (currentMillis - stateTimer >= 2000) {
                testState = 0; 
            }
            break;
    }
}

void setup() {
    Serial.begin(115200);
    ble_setup();
    analogWriteResolution(8); 
    
    pumpL.begin();
    valveL.begin();
    fsrL.begin();
    
    airPressL.begin();
    delay(2000); 
    airPressL.tare(10); 
    ble_log("System Ready!");
}

void loop() {
    ble_loop(); 
    testInflation();
}