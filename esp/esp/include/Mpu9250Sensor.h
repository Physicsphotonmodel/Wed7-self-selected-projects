#ifndef MPU9250_SENSOR_H
#define MPU9250_SENSOR_H

#include <Arduino.h>
#include <Wire.h>

// 定義 MPU9250 的 I2C 預設位址 (Sensor A)
#define MPU9250_ADDR_A 0x68 

struct MpuData {
    int16_t accelX;
    int16_t accelY;
    int16_t accelZ;
    int16_t gyroX;
    int16_t gyroY;
    int16_t gyroZ;
};

class Mpu9250Sensor {
public:
    // 建構子，允許傳入指定的 I2C 位址 (方便以後擴充 Sensor B)
    Mpu9250Sensor(uint8_t i2cAddress = MPU9250_ADDR_A);

    bool init();

    MpuData readData();

private:
    uint8_t _address;
};

#endif