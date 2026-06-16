#include "Arduino.h"
#include "pump.h"       
#include "valve.h"
#include "Hx710Sensor.h"   
#include "bluetooth.h"

const int PIN_PUMP_L = 17;
const int PIN_VALVE_L = 32;


Pump pumpL(PIN_PUMP_L);
Valve valveL(PIN_VALVE_L);
Hx710Sensor hx710L(36,18,0,0,0);

unsigned long stateTimer = 0;
int testState = 0;

void testInflation() {
    unsigned long currentMillis = millis();

    switch (testState) {
        case 0:
            ble_log(">>> State 0: Valve CLOSE, Pump ON (3.5s)");
            valveL.close();      
            pumpL.setpwm(255); 
            stateTimer = currentMillis;
            testState = 1;
            break;

        case 1:
            if (currentMillis - stateTimer >= 3500) {
                // 修改處：改為讀取 Hx710 的數值
                int hxValue = analogRead(36);
                ble_log("HX710 Value: " + String(hxValue));
                
                ble_log(">>> State 1: Valve OPEN, Pump OFF (0.5s)");
                valveL.open();
                pumpL.setpwm(0);    
                
                stateTimer = currentMillis;
                testState = 2;
            }
            break;

        case 2:
            if (currentMillis - stateTimer >= 500) {
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
 hx710L.begin(); 
}

void loop() {
    ble_loop();
    testInflation();
}
