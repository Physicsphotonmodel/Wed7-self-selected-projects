#include "bluetooth.h"
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

// 定義 UUID
const char* SERVICE_UUID        = "4fafc201-1fb5-454e-8a2c-01412e646461";
const char* CHARACTERISTIC_UUID = "beb5483e-36e1-4688-b7f5-ea07361b26a8";

BLEServer* pServer = NULL;
BLECharacteristic* pCharacteristic = NULL;
bool deviceConnected = false;
bool oldDeviceConnected = false;

String latest_string_command = "";


char latest_command = '\0'; // 儲存手機過來的最新指令

// 處理連線狀態的回撥函式
class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
        deviceConnected = true;
        Serial.println("\n[BLE INFO] 手機 nRF Connect 已成功連線！");
    }

    void onDisconnect(BLEServer* pServer) {
        deviceConnected = false;
        Serial.println("\n[BLE INFO] 手機 nRF Connect 已斷開連線！");
    }
};

// 處理手機寫入指令的回撥函式
class MyCharacteristicCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
        std::string rxValue = pCharacteristic->getValue();
        if (rxValue.length() > 0) {
            // 關鍵：將整個字串轉成 String，不要只取第一個字元！
            latest_string_command = String(rxValue.c_str()); 
            Serial.print("\n[BLE RECEIVE] 收到字串輸入: ");
            Serial.println(latest_string_command);
        }
    }
};

void ble_setup() {
    Serial.println("[INFO] BLE 啟動中...");
    BLEDevice::init("Smart_Pillow_System");
    BLEDevice::setMTU(512); 

    pServer = BLEDevice::createServer();
    pServer->setCallbacks(new MyServerCallbacks());

    BLEService *pService = pServer->createService(SERVICE_UUID);

    pCharacteristic = pService->createCharacteristic(
                        CHARACTERISTIC_UUID,
                        BLECharacteristic::PROPERTY_READ   |
                        BLECharacteristic::PROPERTY_WRITE  |
                        BLECharacteristic::PROPERTY_NOTIFY
                      );

    pCharacteristic->addDescriptor(new BLE2902());
    pCharacteristic->setCallbacks(new MyCharacteristicCallbacks());
    
    pService->start();
    
    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(SERVICE_UUID);
    pAdvertising->setScanResponse(false);
    pAdvertising->setMinPreferred(0x0);
    BLEDevice::startAdvertising();
    
    Serial.println("[INFO] 藍牙廣播已開啟，等待手機連線...");
}

void ble_loop() {
    if (!deviceConnected && oldDeviceConnected) {
        delay(500); 
        pServer->startAdvertising(); 
        Serial.println("[BLE INFO] 重新開啟藍牙廣播...");
        oldDeviceConnected = deviceConnected;
    }
    if (deviceConnected && !oldDeviceConnected) {
        oldDeviceConnected = deviceConnected;
    }
}

void ble_log(String message) {
    Serial.println(message);
    if (deviceConnected) {
        pCharacteristic->setValue((uint8_t*)message.c_str(), message.length());
        pCharacteristic->notify();
        delay(10); 
    }
}

bool is_ble_connected() {
    return deviceConnected;
}

String get_ble_string_command() {
    String cmd = latest_string_command;
    latest_string_command = ""; // 讀取後清空
    return cmd;
}

char get_ble_command() {

    if (latest_string_command.length() > 0) return latest_string_command[0];
    
    char cmd = latest_command;
    latest_command = '\0'; // 讀取後清空，避免重複觸發
    return cmd;
}