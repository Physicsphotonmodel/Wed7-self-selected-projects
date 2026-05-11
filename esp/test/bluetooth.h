#ifndef BLUETOOTH_H
#define BLUETOOTH_H

#include <Arduino.h>

#define SERVICE_UUID        "0000181a-0000-1000-8000-00805f9b34fb"
#define CHARACTERISTIC_UUID "00002a58-0000-1000-8000-00805f9b34fb"

void ble_setup();
void ble_loop();

#endif