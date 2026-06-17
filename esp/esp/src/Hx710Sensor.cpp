#include "Hx710Sensor.h"
#include "bluetooth.h"

float MAX_VAL = 3550000;    // 改成欣脖的壓力

Hx710Sensor::Hx710Sensor(uint8_t outPin, uint8_t sckPin, float kp, float ki, float kd) {
    _outPin = outPin;
    _sckPin = sckPin;
    _offset = 0;
    _kp = kp;
    _ki = ki;
    _kd = kd;
    _integral = 0;
    _previousError = 0;
}

void Hx710Sensor::begin() {
    pinMode(_sckPin, OUTPUT);
    pinMode(_outPin, INPUT);
    digitalWrite(_sckPin, LOW); 
}

bool Hx710Sensor::isReady() {
    return digitalRead(_outPin) == LOW;
}

long Hx710Sensor::readRaw() {
    // Watchdog timeout to prevent infinite loop
    unsigned long startWait = millis();
    while (!isReady()) {
        yield(); 
        if (millis() - startWait > 500) {
            Serial.println("[ERROR] HX710 read timeout! Check wiring.");
            return 0;
        }
    }

    long count = 0;
    
    for (int i = 0; i < 24; i++) {
        digitalWrite(_sckPin, HIGH);
        delayMicroseconds(1);
        count = count << 1;
        digitalWrite(_sckPin, LOW);
        delayMicroseconds(1);
        if (digitalRead(_outPin)) {
            count++;
        }
    }

    digitalWrite(_sckPin, HIGH);
    delayMicroseconds(1);
    digitalWrite(_sckPin, LOW);
    delayMicroseconds(1);

    if (count & 0x800000) {
        count |= 0xFF000000; 
    }
    
    return count;
}

long Hx710Sensor::readAverage(uint8_t times) {
    if (times == 0) times = 1;
    long sum = 0;
    for (int i = 0; i < times; i++) {
        sum += readRaw();
    }
    return sum / times;
}

void Hx710Sensor::tare(uint8_t times) {
    readRaw(); readRaw();
    _offset = readAverage(times);
}

long Hx710Sensor::getRelativeValue() {
    long currentRaw = readAverage(5); 
    long diff = currentRaw - _offset;
    diff = map(diff, 0, MAX_VAL, 0, 100);
    return diff;
}

bool Hx710Sensor::updatePID(uint8_t pumpPin, uint8_t valvePin) {
    long current = getRelativeValue();
    long error = 100 - current;
    
    if (error <= 2 && error >= -2) {
        analogWrite(pumpPin, 0);
        digitalWrite(valvePin, HIGH); // Hardware specific NC/NO logic
        return false; 
    }

    if (error > 0) {
        digitalWrite(valvePin, HIGH);
        _integral += error;
        _integral = constrain(_integral, 0, 1000); 
        long derivative = error - _previousError;
        int output = (int)(_kp * error + _ki * _integral + _kd * derivative);
        
        // Drive motor with calculated output
        output = constrain(output, 0, 255);
        analogWrite(pumpPin, output);
    } 
    else {
        analogWrite(pumpPin, 0);     
        digitalWrite(valvePin, LOW); 
    }

    _previousError = error;
    return true;
}

int Hx710Sensor::updatePID_cont(void) {
    long current = getRelativeValue();
    long error = 100 - current;
    
    _integral = constrain(_integral + error, -1000, 1000); 
    long derivative = error - _previousError;
    _previousError = error;

    int output = (int)(_kp * error + _ki * _integral + _kd * derivative);
    
    if (abs(error) <= 1) return 0;
    return constrain(output, 0, 255); 
}

void Hx710Sensor::resetPID() {
    _integral = 0;
    _previousError = 0;
}

void Hx710Sensor::readAndPrint() {
    long value = readRaw();
    ble_log("HX710 Raw Value: " + String(value));
}