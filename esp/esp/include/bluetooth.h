#ifndef BLUETOOTH_H
#define BLUETOOTH_H

#include <Arduino.h>

#define SERVICE_UUID        "0000181A-0000-1000-8000-00805f9b34fb"
#define CHARACTERISTIC_UUID "00002A58-0000-1000-8000-00805f9b34fb"

void ble_setup();
void ble_loop();
void ble_log(String message);

// 新增的互動控制介面
bool is_ble_connected();
char get_ble_command(); 

#endif