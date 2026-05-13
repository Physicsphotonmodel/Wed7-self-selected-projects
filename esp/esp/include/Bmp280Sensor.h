#ifndef BMP280_SENSOR_H
#define BMP280_SENSOR_H

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_BMP280.h>

// GY-91 上 BMP280 的預設 I2C 位址 
// (SAO 接地/懸空時通常是 0x76；SAO 接 3.3V 時會變成 0x77)
#define BMP280_ADDR_A 0x76 

class Bmp280Sensor {
public:
    Bmp280Sensor(uint8_t i2cAddress = BMP280_ADDR_A);

    bool init();

    // 讀取真實氣壓 (單位：Pa，如果要百帕 hPa 要自行除以 100)
    float readPressure();

    float readTemperature();

private:
    uint8_t _address;
    Adafruit_BMP280 _bmp;
};

#endif