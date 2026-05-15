#include <Arduino.h>
#include <Wire.h>
#include "driver.h"
#include "Mpu9250Sensor.h"
#include "Bmp280Sensor.h"
#include "loadcell.h"

#define DEBUG 1

// ==========================================
// Pin Definitions
// ==========================================
// 1. Driver pins (MOS modules)
// const int PIN_MOTOR_L = 18;
// const int PIN_MOTOR_R = 19;
// const int PIN_PUMP_L  = 27;
// const int PIN_PUMP_R  = 14;
// const int PIN_VALVE_L = 12;
// const int PIN_VALVE_R = 13;

// 2. Load cell (HX711) pins
const int PIN_HX_DT_L  = 18;
const int PIN_HX_SCK_L = 19;
// const int PIN_HX_DT_R  = 25;
// const int PIN_HX_SCK_R = 26; 

// ==========================================
// Object Instantiation
// ==========================================
// Actuators
// Driver motorL(PIN_MOTOR_L);
// Driver motorR(PIN_MOTOR_R);
// Driver pumpL(PIN_PUMP_L);
// Driver pumpR(PIN_PUMP_R);
// Driver valveL(PIN_VALVE_L);
// Driver valveR(PIN_VALVE_R);

// I2C Sensors (MPU9250 + BMP280)
Mpu9250Sensor imuL(0x68);
// Mpu9250Sensor imuR(0x69);
Bmp280Sensor  bmpL(0x76);
// Bmp280Sensor  bmpR(0x77);

// Load cells (HX711)
LoadCell loadCellL(PIN_HX_DT_L, PIN_HX_SCK_L);
// LoadCell loadCellR(PIN_HX_DT_R, PIN_HX_SCK_R);

void setup() {
    Serial.begin(115200);
    while (!Serial) { delay(10); } 
    Serial.println("\n=== System Booting ===");

    // 1. Initialize I2C bus (ESP32 default: SDA=21, SCL=22)
    Wire.begin();

    // 2. Initialize all drivers (Default state: OFF)
    // motorL.begin();
    // motorR.begin();
    // pumpL.begin();
    // pumpR.begin();
    // valveL.begin();
    // valveR.begin();
    // Serial.println("[System] Actuators initialized (State: OFF)");

    // 3. Initialize I2C sensors
    Serial.print("[System] IMU L (0x68): "); Serial.println(imuL.init() ? "OK" : "FAIL");
    // Serial.print("[System] IMU R (0x69): "); Serial.println(imuR.init() ? "OK" : "FAIL");
    Serial.print("[System] BMP L (0x76): "); Serial.println(bmpL.init() ? "OK" : "FAIL");
    // Serial.print("[System] BMP R (0x77): "); Serial.println(bmpR.init() ? "OK" : "FAIL");

    // 4. Initialize load cells
    loadCellL.begin();
    // loadCellR.begin();
    Serial.println("[System] Taring load cells...");
    loadCellL.tare(); 
    // loadCellR.tare();
    
    // Uncomment and apply calculated calibration factors later
    // loadCellL.setCalibration(-450.0); 
    // loadCellR.setCalibration(-450.0);

    Serial.println("=== Initialization Complete ===\n");
    delay(2000);
}

void loop() {
    if (DEBUG) {
        // --- Read sensor data ---
        MpuData dataL = imuL.readData();
        // MpuData dataR = imuR.readData();
        float tempL = bmpL.readTemperature();
        float pressL = bmpL.readPressure();
        float weightL = loadCellL.getWeight(3); // Average of 3 readings
        // float weightR = loadCellR.getWeight(3);

        // --- Format and print ---
        Serial.println("--------------------------------------------------");
        
        Serial.print("[IMU L] AccZ: "); Serial.print(dataL.accelZ);
        Serial.print(" \tGyroZ: "); Serial.println(dataL.gyroZ);
        
        // Serial.print("[IMU R] AccZ: "); Serial.print(dataR.accelZ);
        // Serial.print(" \tGyroZ: "); Serial.println(dataR.gyroZ);

        Serial.print("[BMP L] Temp: "); Serial.print(tempL);
        Serial.print(" C \tPress: "); Serial.print(pressL); Serial.println(" Pa");

        Serial.print("[Load L] Value: "); Serial.print(weightL, 1);
        // Serial.print(" \t[Load R] Value: "); Serial.println(weightR, 1);
        
        Serial.println("--------------------------------------------------");
    }

    delay(1000); 
}