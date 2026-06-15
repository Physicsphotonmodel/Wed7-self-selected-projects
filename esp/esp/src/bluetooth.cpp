#include "bluetooth.h"
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

BLEServer* pServer = NULL;
BLECharacteristic* pCharacteristic = NULL;
bool deviceConnected = false;
bool oldDeviceConnected = false;

char latest_command = '\0'; // 儲存手機傳來的最新指令

// 處理連線狀態的回呼
class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
      Serial.println("nRF Connect 已連線！");
    };

    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
      Serial.println("nRF Connect 已斷線！");
    }
};

// ? 新增：處理手機「寫入」指令的回呼
class MyCharacteristicCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
        std::string rxValue = pCharacteristic->getValue();
        if (rxValue.length() > 0) {
            // 只抓取第一個字元（例如 '1' 或 '0'）
            latest_command = rxValue[0]; 
            Serial.print("藍牙收到原始訊號: ");
            Serial.println(latest_command);
        }
    }
};

void ble_setup() {
  Serial.println("BLE 啟動中...");
  BLEDevice::init("Smart_Neck_Pillow");
  BLEDevice::setMTU(512); 

  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  BLEService *pService = pServer->createService(SERVICE_UUID);

  // ? 加入 PROPERTY_WRITE 權限
  pCharacteristic = pService->createCharacteristic(
                      CHARACTERISTIC_UUID,
                      BLECharacteristic::PROPERTY_READ   |
                      BLECharacteristic::PROPERTY_WRITE  |
                      BLECharacteristic::PROPERTY_NOTIFY
                    );

  pCharacteristic->addDescriptor(new BLE2902());
  // ? 綁定接收指令的回呼函式
  pCharacteristic->setCallbacks(new MyCharacteristicCallbacks());
  
  pService->start();
  
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(false);
  pAdvertising->setMinPreferred(0x0);
  BLEDevice::startAdvertising();
  
  Serial.println("BLE 廣播已啟動，等待連線...");
}

void ble_loop() {
  if (!deviceConnected && oldDeviceConnected) {
      delay(500); 
      pServer->startAdvertising(); 
      Serial.println("BLE 重新開始廣播...");
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

// 給主程式呼叫用的：讀取指令後立刻清空，避免重複執行
char get_ble_command() {
    char cmd = latest_command;
    latest_command = '\0'; 
    return cmd;
}