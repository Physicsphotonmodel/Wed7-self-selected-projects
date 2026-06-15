#ifndef BLUETOOTH_H
#define BLUETOOTH_H

#include <Arduino.h>

// 全域變數宣告（告訴主程式這兩個外部變數存在）
extern const char* SERVICE_UUID;
extern const char* CHARACTERISTIC_UUID;
String get_ble_string_command();

// 供主程式呼叫的藍牙功能函式介面
void ble_setup();
void ble_loop();
void ble_log(String message);

// 供主程式確認連線與獲取手機指令的介面
bool is_ble_connected();
char get_ble_command(); 

#endif