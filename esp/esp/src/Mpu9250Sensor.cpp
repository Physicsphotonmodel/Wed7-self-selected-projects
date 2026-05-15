#include "Mpu9250Sensor.h"

Mpu9250Sensor::Mpu9250Sensor(uint8_t i2cAddress) {
    _address = i2cAddress;
}

bool Mpu9250Sensor::init() {
    Wire.beginTransmission(_address);
    Wire.write(0x6B);
    Wire.write(0x00);
    
    if (Wire.endTransmission() == 0) {
        return true;
    }
    return false; 
}

MpuData Mpu9250Sensor::readData() {
    MpuData data = {0, 0, 0, 0, 0, 0};

    Wire.beginTransmission(_address);
    Wire.write(0x3B);
    Wire.endTransmission(false);
    
    Wire.requestFrom(_address, (uint8_t)14);

    if (Wire.available() >= 14) {
        data.accelX = (Wire.read() << 8) | Wire.read();
        data.accelY = (Wire.read() << 8) | Wire.read();
        data.accelZ = (Wire.read() << 8) | Wire.read();
        
        Wire.read(); 
        Wire.read(); 

        data.gyroX = (Wire.read() << 8) | Wire.read();
        data.gyroY = (Wire.read() << 8) | Wire.read();
        data.gyroZ = (Wire.read() << 8) | Wire.read();
    }
    return data;
}