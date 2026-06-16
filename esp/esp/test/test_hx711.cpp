#include "Arduino.h"
#include "pump.h"       
#include "valve.h"
#include "PressureSensor.h"
#include "Hx710Sensor.h"   
#include "bluetooth.h"

const int PIN_PUMP_L = 17;
const int PIN_VALVE_L = 32;
const int PIN_OUT_L = 19;

Pump pumpL(PIN_PUMP_L);
Valve valveL(PIN_VALVE_L);
PressureSensor SensorL(PIN_OUT_L);

/**
 * @brief 測試充氣並記錄氣壓 (測試用 Function)
 */
void testInflation() {
    
    ble_log(">>> initial 0.5s...");
    valveL.close();      
    pumpL.setpwm(255); 
    delay(3500);  
    
    int val = analogRead(PIN_OUT_L);
    ble_log(String(val));
    valveL.open();
    
    pumpL.setpwm(0);    
    delay(500);

}

void setup() {
    ble_setup();
    pumpL.begin();
    valveL.begin();
    testInflation();
}

void loop() {
    testInflation();
}