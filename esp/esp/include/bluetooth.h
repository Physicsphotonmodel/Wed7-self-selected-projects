#ifndef BLUETOOTH_H
#define BLUETOOTH_H

#include <Arduino.h>

extern const char* SERVICE_UUID;
extern const char* CHARACTERISTIC_UUID;
String get_ble_string_command();

void ble_setup();
void ble_loop();
void ble_log(String message);

bool is_ble_connected();
char get_ble_command(); 

#endif