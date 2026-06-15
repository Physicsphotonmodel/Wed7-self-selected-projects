#include "Hx710Sensor.h"

Hx710Sensor::Hx710Sensor(uint8_t outPin, uint8_t sckPin) {
    _outPin = outPin;
    _sckPin = sckPin;
    _offset = 0;
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
    while (!isReady()) {
        yield(); 
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

    // 發送第 25 個時脈 (10Hz 模式)
    digitalWrite(_sckPin, HIGH);
    delayMicroseconds(1);
    digitalWrite(_sckPin, LOW);
    delayMicroseconds(1);

    // ? 修正：標準的 24-bit 轉 32-bit 二補數轉換 (解決數值亂跳)
    if (count & 0x800000) {
        count |= 0xFF000000; 
    }
    
    return count;
}

// ? 新增：均值濾波器 (抑制高頻雜訊)
long Hx710Sensor::readAverage(uint8_t times) {
    long sum = 0;
    for (int i = 0; i < times; i++) {
        sum += readRaw();
    }
    return sum / times;
}

void Hx710Sensor::tare(uint8_t times) {
    // 丟棄前幾筆不穩定的資料
    readRaw(); readRaw();
    // 讀取平均值作為新的 0 基準線
    _offset = readAverage(times);
}

long Hx710Sensor::getRelativeValue() {
    // 1. 取得當下過濾後的平均值
    long currentRaw = readAverage(5); 
    
    // 2. 扣除初始大氣壓基準線
    long diff = currentRaw - _offset;
    
    // 3. ? 強制反轉斜率：解決硬體 A+/A- 反接導致的負相關問題
    // 如果你發現吸氣變正、吹氣變負，就保留這個 -1。如果相反，就拿掉 -1。
    return diff * -1; 
}