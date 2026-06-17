#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <Arduino.h>
#include "Hx710Sensor.h"
#include "PressureSensor.h"
#include "pump.h"
#include "valve.h"


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

    float target_pressure;

public:
    Controller();

    void begin();
    void setTargetPressure(float base_pressure);

    void PIDcontrol();

    void singleSideInflation();

    void overshoot_deal();
};

#endif