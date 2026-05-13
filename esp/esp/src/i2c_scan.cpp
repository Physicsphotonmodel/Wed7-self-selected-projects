// #include <Arduino.h>
// #include <Wire.h>

// void setup() {
//   Wire.begin();
//   Serial.begin(115200);
//   Serial.println("\nI2C 雷達掃描開始...");
// }

// void loop() {
//   byte error, address;
//   int nDevices = 0;

//   Serial.println("掃描中...");

//   for(address = 1; address < 127; address++ ) {
//     Wire.beginTransmission(address);
//     error = Wire.endTransmission();

//     if (error == 0) {
//       Serial.print("找到 I2C 裝置，位址是: 0x");
//       if (address<16) Serial.print("0");
//       Serial.println(address,HEX);
//       nDevices++;
//     }
//   }
//   if (nDevices == 0)
//     Serial.println("找不到任何 I2C 裝置\n");
//   else
//     Serial.println("掃描完畢\n");

//   delay(5000); // 每5秒掃一次
// }