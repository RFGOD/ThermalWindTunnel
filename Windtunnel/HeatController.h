#ifndef HEAT_CONTROLLER_H
#define HEAT_CONTROLLER_H

#include <Arduino.h>

class HeatController {
  public:
    HeatController(int heatpwmPin);
    void begin();
    void setHeatSpeed(int heatspeed);

  private:
    int _heatpwmPin;
    int _heatSpeed;
    void updateheatPWM();
    int relay = 11;
};

#endif