#include <Arduino.h>
#include "bluetooth.h"

void setup() {

    ble_setup();
}

void loop() {
    ble_loop();

    if (is_ble_connected()) {
        ble_log("System is running...");
        ble_log(String(30000));

    }

    delay(1000); 
}
