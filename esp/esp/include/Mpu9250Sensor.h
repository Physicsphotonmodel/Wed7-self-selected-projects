#ifndef MPU9250_SENSOR_H
#define MPU9250_SENSOR_H

#include <Arduino.h>
#include <Wire.h>

#define MPU9250_ADDR_LEFT  0x68 
#define MPU9250_ADDR_RIGHT 0x69 

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
    Mpu9250Sensor(uint8_t i2cAddress);

    bool init();

    MpuData readData();

private:
    uint8_t _address;
};

#endif