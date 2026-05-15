#ifndef BMP280_SENSOR_H
#define BMP280_SENSOR_H

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_BMP280.h>

#define BMP280_ADDR_LEFT  0x76 // SAO 懸空或接地
#define BMP280_ADDR_RIGHT 0x77 // SAO 接 3.3V

class Bmp280Sensor {
public:
    Bmp280Sensor(uint8_t i2cAddress);

    bool init();

    float readPressure();

    float readTemperature();

private:
    uint8_t _address;
    Adafruit_BMP280 _bmp;
};

#endif