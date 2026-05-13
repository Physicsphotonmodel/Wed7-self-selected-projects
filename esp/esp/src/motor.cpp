// #include <Arduino.h>
// const int motorPin = 18;

// void setup() {
//   Serial.begin(115200);
//   Serial.println("MOS motor unit test");

//   pinMode(motorPin, OUTPUT);
// }

// void loop() {
//   Serial.println("100%");
//   digitalWrite(motorPin, HIGH);
//   delay(2000);

//   Serial.println("0%");
//   digitalWrite(motorPin, LOW);
//   delay(2000);

//   Serial.println("Stronger..");
//   for (int dutyCycle = 0; dutyCycle <= 255; dutyCycle += 5) {
//     analogWrite(motorPin, dutyCycle); 
//     delay(50);
//   }

//   Serial.println("Weaker...");
//   for (int dutyCycle = 255; dutyCycle >= 0; dutyCycle -= 5) {
//     analogWrite(motorPin, dutyCycle);
//     delay(50);
//   }

//   Serial.println("END");
//   delay(3000); 
// }