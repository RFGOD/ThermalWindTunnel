#ifndef STEPPER_MOTOR_CONTROLLER_H
#define STEPPER_MOTOR_CONTROLLER_H

#include <Arduino.h>

class StepperMotorController {
public: 
  StepperMotorController(int dirPin, int stepPin, int limLow, int limHigh);
  void begin();
  void homeMotor();
  void moveStepper();
  void setDirection(bool dir);
  int getPosition() const;

private:
  int directionPin;
  int stepPin;
  int limitLowPin;
  int limitHighPin;
  int position = 0;
  int home = 0;
  bool motorsHomed = false;

  int checkLimits();
};

#endif