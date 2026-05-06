#ifndef HEART_RATE_MONITOR_H
#define HEART_RATE_MONITOR_H

#include <Wire.h>
#include "MAX30100_PulseOximeter.h"

class HeartRateMonitor {

private:
    PulseOximeter pox;  // MAX30100 oximeter instance
    uint32_t tsLastReport; // last report timestamp
    uint32_t reportPeriod; // report period in milliseconds
    int zerocount = 0; // count of consecutive zero heart rate readings
    bool isTerminated; // flag to indicate if the monitor is terminated
    
    void printData() {
        Serial.print("Heart rate:");
        Serial.print(pox.getHeartRate());
        Serial.print("bpm / SpO2:");
        Serial.print(pox.getSpO2());
        Serial.println("%");
    }

public:
    HeartRateMonitor(uint32_t period = 1000) 
        : tsLastReport(0), reportPeriod(period) {}

    bool begin() {
        Serial.print("Initializing pulse oximeter..");
        if (!pox.begin()) {
            Serial.println("FAILED");
            return false;
        }
        Serial.println("SUCCESS");
        return true;
    }

    // set callback for beat detection
    void setBeatCallback(void (*callback)()) {
        pox.setOnBeatDetectedCallback(callback);
    }

    // update function to be called in the main loop
    void update() {

        if(isTerminated) {return;} // skip update if terminated
        pox.update();

        if (millis() - tsLastReport > reportPeriod) {
            printData();
            
            if (pox.getHeartRate() == 0) {zerocount++;} 
            else {zerocount = 0; }
            
            if (zerocount >= 40) {terminate();}
            tsLastReport = millis();
        }
    }



    // get data interface for easy extension 
    //(e.g., sending to Bluetooth or display)
    float getHR() { return pox.getHeartRate(); }
    uint8_t getSpO2() { return pox.getSpO2(); }
    void setReportPeriod(uint32_t period) { reportPeriod = period; }

    void terminate() {
        isTerminated = true;
        Serial.println("Heart rate monitor terminated due to consecutive zero readings.");
    }
};

#endif