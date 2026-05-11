#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

BLEServer* pServer = NULL;
BLECharacteristic* pCharacteristic = NULL;
bool deviceConnected = false;
bool oldDeviceConnected = false;

// 模擬用的假資料變數
int16_t mockLeftAngle = 0;
int16_t mockRightAngle = 0;
int16_t mockLeftPressure = 1000;
int16_t mockRightPressure = 1000;
uint8_t mockState = 0;

// BLE 連線狀態回呼函數
class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
      Serial.println("App 已連線！");
    };

    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
      Serial.println("App 已斷線！");
    }
};

void setup() {
  Serial.begin(115200);

  // 1. 初始化 BLE 裝置
  BLEDevice::init("Smart_Neck_Pillow");

  // 2. 建立 BLE Server
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // 3. 建立 BLE Service
  BLEService *pService = pServer->createService(SERVICE_UUID);

  // 4. 建立 BLE Characteristic (設定為可讀取與可推播 Notify)
  pCharacteristic = pService->createCharacteristic(
                      CHARACTERISTIC_UUID,
                      BLECharacteristic::PROPERTY_READ   |
                      BLECharacteristic::PROPERTY_NOTIFY
                    );

  // 5. 加入 Descriptor (這是讓 App 端可以訂閱 Notify 的關鍵)
  pCharacteristic->addDescriptor(new BLE2902());

  // 6. 啟動 Service 並開始廣播
  pService->start();
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(false);
  pAdvertising->setMinPreferred(0x0);
  BLEDevice::startAdvertising();
  Serial.println("BLE 廣播中... 等待 Android App 連線");
}

void loop() {
  // 如果 App 有連線，就開始狂發資料
  if (deviceConnected) {
    
    // --- 這裡放入你未來的感測器讀取邏輯 ---
    // 目前先讓假角度遞增，模擬頭部轉動
    mockLeftAngle += 1;
    if(mockLeftAngle > 90) mockLeftAngle = -90;
    // --------------------------------------

    // 打包資料：總共 9 bytes
    // 格式: [State(1)] [LeftAngle(2)] [RightAngle(2)] [LeftPressure(2)] [RightPressure(2)]
    uint8_t payload[9];
    payload[0] = mockState;
    payload[1] = mockLeftAngle & 0xFF;         // Low byte
    payload[2] = (mockLeftAngle >> 8) & 0xFF;  // High byte
    payload[3] = mockRightAngle & 0xFF;
    payload[4] = (mockRightAngle >> 8) & 0xFF;
    payload[5] = mockLeftPressure & 0xFF;
    payload[6] = (mockLeftPressure >> 8) & 0xFF;
    payload[7] = mockRightPressure & 0xFF;
    payload[8] = (mockRightPressure >> 8) & 0xFF;

    // 設定值並發送 Notify
    pCharacteristic->setValue(payload, sizeof(payload));
    pCharacteristic->notify();

    // 延遲 50 毫秒 (大約 20Hz 的更新率，對 3D 渲染足夠了)
    delay(50);
  }

  // 處理斷線與重連的邏輯
  if (!deviceConnected && oldDeviceConnected) {
      delay(500); 
      pServer->startAdvertising(); 
      Serial.println("重新開始廣播");
      oldDeviceConnected = deviceConnected;
  }
  if (deviceConnected && !oldDeviceConnected) {
      oldDeviceConnected = deviceConnected;
  }
}