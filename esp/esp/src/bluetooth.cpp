#include "bluetooth.h"
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

#define SERVICE_UUID        "0000181A-0000-1000-8000-00805f9b34fb"
#define CHARACTERISTIC_UUID "00002A58-0000-1000-8000-00805f9b34fb"

BLEServer* pServer = NULL;
BLECharacteristic* pCharacteristic = NULL;
bool deviceConnected = false;
bool oldDeviceConnected = false;

uint16_t mockValue = 0;

class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
      Serial.println("App 已連線");
    };

    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
      Serial.println("App 已斷線");
    }
};

void ble_setup() {
  Serial.begin(115200);
  Serial.println("ESP32 啟動中...");

  BLEDevice::init("Smart_Neck_Pillow");
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  BLEService *pService = pServer->createService(SERVICE_UUID);

  pCharacteristic = pService->createCharacteristic(
                      CHARACTERISTIC_UUID,
                      BLECharacteristic::PROPERTY_READ   |
                      BLECharacteristic::PROPERTY_NOTIFY
                    );

  pCharacteristic->addDescriptor(new BLE2902());
  pService->start();
  
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(false);
  pAdvertising->setMinPreferred(0x0);
  BLEDevice::startAdvertising();
  
  Serial.println("BLE 廣播已啟動，等待連線...");
}

void ble_loop() {
  if (deviceConnected) {
    mockValue += 5;
    if(mockValue > 1000) mockValue = 0;

    Serial.print("ESP32 內部準備發送的值: ");
    Serial.println(mockValue);

    uint8_t payload[2];
    payload[0] = mockValue & 0xFF;         // Low byte
    payload[1] = (mockValue >> 8) & 0xFF;  // High byte
    
    pCharacteristic->setValue(payload, sizeof(payload));
    pCharacteristic->notify(); 

    delay(500);
  }

  if (!deviceConnected && oldDeviceConnected) {
      delay(500); 
      pServer->startAdvertising(); 
      Serial.println("重新開始廣播...");
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