#include <Arduino.h>
#include "Hx710Sensor.h"
#include "bluetooth.h"


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


Hx710Sensor pressureSensor(PIN_HX710_OUT_L, PIN_HX710_OUT_L, 0, 0, 0);

void setup() {
    ble_setup(); // 初始化藍牙
    pressureSensor.begin();
    
    while (!is_ble_connected()) { delay(500); }
    ble_log("--- HX710 Pressure System Starting ---");
    
    // 1. 自動歸零
    ble_log("Step 1: Taring... Please remove all pressure.");
    delay(2000);
    pressureSensor.tare(10);
    ble_log("Tare completed.");

    
    ble_log("Calibration finished. Starting PID Control.");
}

void loop() {
    
    ble_loop();
    long currentPressure = pressureSensor.readRaw();
    
    static unsigned long lastLogTime = 0;
    if (millis() - lastLogTime > 1000) {
        ble_log("Raw data pressure: " + String(currentPressure));
        lastLogTime = millis();
    }
}