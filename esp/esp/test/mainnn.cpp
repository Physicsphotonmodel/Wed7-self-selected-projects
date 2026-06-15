#include <Arduino.h>
#include "bluetooth.h"

// the class of a controller

struct PID_Controller {
    float kp = 1.2; float ki = 0.2; float kd = 0.1;
    float integral = 0; float prev_error = 0;
    void reset() { integral = 0; prev_error = 0; }
};

PID_Controller myPID;
float current_pressure = 70.0;
float target_pressure = 60.0; // 預設無人時為 60
bool isHeadDetected = false;  // 模擬狀態：false = 無人，true = 有人

unsigned long lastUpdateTick = 0;
int loopCounter = 0;
bool waitingForInput = false;

int runPIDControl(float current, float target, PID_Controller &pid) {
    float error = target - current;
    pid.integral += error * 0.1;
    pid.integral = constrain(pid.integral, -150, 150);
    float derivative = (error - pid.prev_error) / 0.1;
    pid.prev_error = error;
    return constrain((int)((pid.kp * error) + (pid.ki * pid.integral) + (pid.kd * derivative)), 0, 255);
}

void setup() {
    Serial.begin(115200);
    ble_setup();
    lastUpdateTick = millis();
    Serial.println("System Ready. Head Detection Simulation Active.");
}

void loop() {
    ble_loop();
    unsigned long currentMillis = millis();

    // 處理藍牙指令
    // 輸入 '0' -> 模擬頭離開 (60mmHg), 輸入 '1' -> 模擬頭躺下 (90mmHg)
    // 輸入其他數字 (如 75.0) -> 手動設定當前壓力
    // --- A. 處理藍牙輸入 ---
    String cmd = get_ble_string_command(); 
    if (cmd.length() > 0) {
        cmd.trim(); // 【關鍵】移除字串前後的空白與換行符號
        
        Serial.print("Debug: Cleaned cmd = '");
        Serial.print(cmd);
        Serial.println("'");

        if (cmd == "0") {
            isHeadDetected = false;
            target_pressure = 60.0;
            myPID.reset();
            ble_log("Head removed. Target: 60 mmHg");
        } 
        else if (cmd == "1") {
            isHeadDetected = true;
            target_pressure = 90.0;
            myPID.reset();
            ble_log("Head detected. Target: 90 mmHg");
        } 
        else {
            float inputVal = cmd.toFloat();
            // 如果輸入是 100，這裡是有效的數值解析
            if (inputVal >= 50.0 && inputVal <= 150.0) {
                current_pressure = inputVal;
                ble_log("Manual pressure set to: " + String(current_pressure));
            } else {
                ble_log("Invalid Range: " + cmd);
            }
        }
    }
            

    if ((currentMillis - lastUpdateTick >= 500)) {
        lastUpdateTick = currentMillis;

        int motorPWM = runPIDControl(current_pressure, target_pressure, myPID);
        
        // 物理模型：充氣 vs 洩漏
        float inflation = (motorPWM / 255.0) * 4.0;  // mock this is for test
                                                    // 需要一個，給訂多少氣壓，需要再沖給多少wpm的演算法。
                                                    //參數 : 需要在充氣的時候沖到多少，洩氣的時候洩到多少，
                                                    //然後根據這兩個參數來決定充氣的速度。
        float leakage = 0.04 * current_pressure; 
        current_pressure += (inflation - leakage);
        current_pressure = constrain(current_pressure, 0, 150);

        // 格式化輸出
        String logMsg = (isHeadDetected ? "[Head ON] " : "[Head OFF] ") +
                        String(current_pressure, 2) + "mmHg / PWM:" + String(motorPWM);
        
        Serial.println(logMsg);
        ble_log(logMsg);
    }
}