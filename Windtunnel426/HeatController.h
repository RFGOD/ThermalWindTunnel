#ifndef HEAT_CONTROLLER_H
#define HEAT_CONTROLLER_H

#include <Arduino.h>

class HeatController {
  public:
    HeatController(int heatPin);
    void HeatBegin();
    void setHeat(int heat);

  private:
    int _heatPin;
    int _heat;
    void updateHeatPWM();
};

#endif