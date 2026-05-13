
/***************************************************************************
  修正版：BMP280 I2C 單元測試 (基於 image_551058.png 接線)
  接線說明：
  - VIN -> 5V (或 3.3V)
  - GND -> GND
  - SCL -> A5 (綠線)
  - SDA -> A4 (藍線)
 ***************************************************************************/

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>

// 建立感測器實體 (使用 I2C)
Adafruit_BMP280 bmp; 

void setup() {
  Serial.begin(9600);
  while (!Serial) delay(100); // 等待序列埠開啟
  
  Serial.println(F("--- BMP280 運作測試 ---"));

  /* 
   * 測試 1: 檢查感測器連線 
   * 備註：有些廉價模組的 I2C 位址是 0x76 而非預設的 0x77
   */
  if (!bmp.begin(0x76)) { 
    Serial.println(F("錯誤: 找不到 BMP280 感測器！"));
    Serial.println(F("1. 請檢查 A4/A5 接線是否鬆脫。"));
    Serial.println(F("2. 嘗試將 bmp.begin(0x76) 改為 (0x77)。"));
    while (1); 
  }

  /* 測試 2: 設定感測器參數 */
  Serial.println(F("感測器初始化成功。"));
  bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,     /* 運作模式 */
                  Adafruit_BMP280::SAMPLING_X2,     /* 溫度過採樣 */
                  Adafruit_BMP280::SAMPLING_X16,    /* 壓力過採樣 */
                  Adafruit_BMP280::FILTER_X16,      /* 濾波器 */
                  Adafruit_BMP280::STANDBY_MS_500); /* 待機時間 */
}

void loop() {
  // 讀取數據
  float temp = bmp.readTemperature();
  float press = bmp.readPressure();
  float alt = bmp.readAltitude(1013.25); // 請根據當地氣壓修正

  // 測試 3: 數據合理性檢查 (簡單的 Unit Test 邏輯)
  Serial.print(F("[TEST] "));
  if (isnan(temp) || temp < -40 || temp > 85) {
    Serial.print(F("溫度異常! "));
  } else {
    Serial.print(F("溫度正常: "));
    Serial.print(temp);
    Serial.print(F(" *C | "));
  }

  Serial.print(F("壓力: "));
  Serial.print(press);
  Serial.print(F(" Pa | "));

  Serial.print(F("高度: "));
  Serial.print(alt);
  Serial.println(F(" m"));

  delay(2000);
}