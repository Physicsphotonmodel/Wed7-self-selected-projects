#include "pox.h"

HeartRateMonitor hrMonitor(1000);

void onPulse() {
    Serial.println(" [Beat!] ♥");
}

void setup() {

    Serial.begin(115200);
    while (!Serial);

    Serial.println("\n--- System Starting ---");

    if (!hrMonitor.begin()) {
        Serial.println("System halt: Sensor not found.");
        while (1); //stop execution if sensor initialization fails
    }

    hrMonitor.setBeatCallback(onPulse);
    Serial.println("Place your finger gently on the sensor.");
}

void loop() {
    hrMonitor.update(); // including termination check inside update
}