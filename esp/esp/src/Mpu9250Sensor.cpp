#include "Mpu9250Sensor.h"

#define PWR_MGMT_1   0x6B
#define ACCEL_XOUT_H 0x3B

Mpu9250Sensor::Mpu9250Sensor(uint8_t i2cAddress) {
    _address = i2cAddress;
}

bool Mpu9250Sensor::init() {
    Wire.begin(); 
    
    Wire.beginTransmission(_address);
    Wire.write(0x6B);
    Wire.write(0x00); 
    if (Wire.endTransmission() != 0) return false; 
    
    delay(10);
    Wire.beginTransmission(_address);
    Wire.write(0x6A); 
    Wire.write(0x00);
    Wire.endTransmission();
    
    delay(10);

    Wire.beginTransmission(_address);
    Wire.write(0x37); 
    Wire.write(0x02);
    Wire.endTransmission();
    
    delay(100); 
    return true;
}

MpuData Mpu9250Sensor::readData() {
    MpuData data;
    
    Wire.beginTransmission(_address);
    Wire.write(ACCEL_XOUT_H);
    Wire.endTransmission(false);

    Wire.requestFrom((uint8_t)_address, (uint8_t)14, (uint8_t)true);

    if (Wire.available() == 14) {
        data.accelX = (Wire.read() << 8) | Wire.read();
        data.accelY = (Wire.read() << 8) | Wire.read();
        data.accelZ = (Wire.read() << 8) | Wire.read();
        
        Wire.read(); 
        Wire.read();
        
        data.gyroX = (Wire.read() << 8) | Wire.read();
        data.gyroY = (Wire.read() << 8) | Wire.read();
        data.gyroZ = (Wire.read() << 8) | Wire.read();
    } else {
        data.accelX = data.accelY = data.accelZ = 0;
        data.gyroX = data.gyroY = data.gyroZ = 0;
    }

    return data;
}