#ifndef FAN_CONTROLLER_H
#define FAN_CONTROLLER_H

#include <Arduino.h>

class FanController {
  public:
    FanController(int pwmPin);
    void begin();
    void setWindSpeed(int speed);

  private:
    int _pwmPin;
    int _windSpeed;
    void updatePWM();
};

#endif