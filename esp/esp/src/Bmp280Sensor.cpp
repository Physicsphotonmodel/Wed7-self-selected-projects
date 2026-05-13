#include "Bmp280Sensor.h"

Bmp280Sensor::Bmp280Sensor(uint8_t i2cAddress) {
    _address = i2cAddress;
}

bool Bmp280Sensor::init() {
    // 預設 Adafruit 使用的晶片 ID 是 0x58，GY-91 通常也是這個
    if (!_bmp.begin(_address)) {
        return false;
    }

    _bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,     // 連續測量模式
                     Adafruit_BMP280::SAMPLING_X2,     // 溫度採樣率
                     Adafruit_BMP280::SAMPLING_X16,    // 氣壓採樣率 (最高精準度)
                     Adafruit_BMP280::FILTER_X16,      // 數位濾波器 (防風吹震動)
                     Adafruit_BMP280::STANDBY_MS_500); // 待機時間
                     
    return true;
}

float Bmp280Sensor::readPressure() {
    return _bmp.readPressure();
}

float Bmp280Sensor::readTemperature() {
    return _bmp.readTemperature();
}