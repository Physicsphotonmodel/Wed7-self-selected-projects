#include "Bmp280Sensor.h"

Bmp280Sensor::Bmp280Sensor(uint8_t i2cAddress) {
    _address = i2cAddress;
}

bool Bmp280Sensor::init() {
    if (!_bmp.begin(_address)) {
        return false;
    }
    _bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,     /* 運作模式 */
                     Adafruit_BMP280::SAMPLING_X2,     /* 溫度採樣率 */
                     Adafruit_BMP280::SAMPLING_X16,    /* 壓力採樣率 */
                     Adafruit_BMP280::FILTER_X16,      /* 濾波器設定 */
                     Adafruit_BMP280::STANDBY_MS_500); /* 待機時間 */
                     
    return true;
}

float Bmp280Sensor::readPressure() {
    return _bmp.readPressure();
}

float Bmp280Sensor::readTemperature() {
    return _bmp.readTemperature();
}