#include "Arduino.h"
#include "pump.h"       
#include "valve.h"
#include "PressureSensor.h"
#include "Hx710Sensor.h"   
#include "bluetooth.h"

// Pin definitions
const int PIN_PUMP_L = 17;
const int PIN_VALVE_L = 32;
const int PIN_FSR_L = 34;          // FSR pin
const int PIN_HX710_OUT_L = 4;     // HX710 OUT pin
const int PIN_HX710_SCK_L = 18;    // HX710 SCK pin

// Instantiate objects
Pump pumpL(PIN_PUMP_L);
Valve valveL(PIN_VALVE_L);
PressureSensor fsrL(PIN_FSR_L);

// Initialize pressure sensor (PID params = 0 for read-only test)
Hx710Sensor airPressL(PIN_HX710_OUT_L, PIN_HX710_SCK_L, 1.0, 0, 0);

unsigned long stateTimer = 0;
int testState = 0;

void testInflation() {
    unsigned long currentMillis = millis();

    switch (testState) {
        case 0:
            // Keep string under 20 bytes to avoid BLE truncation
            ble_log("S0:V_CLS, P_ON"); 
            valveL.close();      
            pumpL.setpwm(255); 
            stateTimer = currentMillis;
            testState = 1;
            break;

        case 1:
            if (currentMillis - stateTimer >= 3500) {
                // 1. Read FSR value
                int fsrVal = fsrL.readRaw(); 
                
                // 2. Read relative air pressure (internally averaged)
                long airVal = airPressL.getRelativeValue();

                // 3. Combine into short string
                String msg = "F:" + String(fsrVal) + " A:" + String(airVal);
                ble_log(msg);
                
                ble_log("S1:V_OPN, P_OFF");
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
    analogWriteResolution(8); // Ensure 255 is max speed
    
    pumpL.begin();
    valveL.begin();
    fsrL.begin();
    
    // Start pressure sensor and tare
    airPressL.begin();
    delay(2000); // Hardware stabilization wait
    airPressL.tare(10); 
    ble_log("System Ready!");
}

void loop() {
    ble_loop(); 
    testInflation();
}