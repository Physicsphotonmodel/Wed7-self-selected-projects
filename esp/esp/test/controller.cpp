#include <Arduino.h>
#include "Hx710Sensor.h"
#include "PressureSensor.h"
#include "pump.h"
#include "controller.h"
#include "valve.h"
#include "constants.h"


class Controller {

private:

    Hx710Sensor sensorL;
    Hx710Sensor sensorR;
    Pump pumpL;
    Pump pumpR;
    Valve valveL;
    Valve valveR;
    PressureSensor pressL;
    PressureSensor pressR;

    float target_pressure = 100;

public:

    Controller() : 
        sensorL(PIN_HX710_OUT_L, PIN_HX710_SCK_L, 0.5, 0.01, 0.2),
        sensorR(PIN_HX710_OUT_R, PIN_HX710_SCK_R, 0.5, 0.01, 0.2),
        pumpL(PIN_PUMP_L),
        pumpR(PIN_PUMP_R),
        pressL(PIN_FSR_L),
        pressR(PIN_FSR_R),
        valveL(PIN_VALVE_L),
        valveR(PIN_VALVE_R) 
    {
    }

    void begin() {
        // 設定 Pin Mode
        pinMode(PIN_PUMP_L, OUTPUT);
        pinMode(PIN_PUMP_R, OUTPUT);
        pinMode(PIN_VALVE_L, OUTPUT);
        pinMode(PIN_VALVE_R, OUTPUT);
        pinMode(PIN_FSR_L, INPUT);
        pinMode(PIN_FSR_R, INPUT);
        sensorL.begin();
        sensorR.begin();
    }


    void setTargetPressure(float base_pressure){
        target_pressure = base_pressure;
    }


    void PIDcontrol() {

        float L_val = sensorL.updatePID(PIN_PUMP_L);
        float R_val = sensorR.updatePID(PIN_PUMP_R);

        if(L_val > 0){
            pumpL.on();
            valveL.close();
        } 
        else{
            pumpL.off(); 
            valveL.open();
        }

        if(R_val > 0){
            pumpR.on();
            valveR.close();
        } 
        
        else {
            pumpR.off();
            valveR.open();
        }

    }

        void singleSideInflation(){
            if(pressL.isPressed() && !pressR.isPressed()){
                pumpL.on();
                pumpR.off();
                valveL.close();
                valveR.close();
            } 
            
            else if(!pressL.isPressed() && pressR.isPressed()){
                pumpL.off();
                pumpR.on();
                valveL.close();
                valveR.close();
            } 
        };

        void overshoot_deal(){

            if(sensorL.getRelativeValue() > target_pressure + 10){
                pumpL.off();
                valveL.open();
            }

            if(sensorR.getRelativeValue() > target_pressure + 10){
                pumpR.off();
                valveR.open();
            }
        }

    };

