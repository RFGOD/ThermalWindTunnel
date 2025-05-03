#include "HeatController.h"

HeatController::HeatController(int heatpwmPin) {
  _heatpwmPin = heatpwmPin;
  _heatSpeed = 0;
}

void HeatController::begin() {
  pinMode(_heatpwmPin, OUTPUT);
  pinMode(relay, OUTPUT);
  analogWriteResolution(16); // 16-bit PWM
  analogWrite(_heatpwmPin, 0);  // Start fan off
}

void HeatController::setHeatSpeed(int speed) {
  _heatSpeed = constrain(speed, 0, 70);
  if(_heatSpeed == 0) {
    digitalWrite(relay, LOW);
  }
  else {
    digitalWrite(relay, HIGH);
  }
  updateheatPWM();
}

void HeatController::updateheatPWM() {
  float heatduty = map(_heatSpeed, 0, 70, 0, 65535); // analogWrite uses 0–65535 giga boads can support 16-bits
  analogWrite(_heatpwmPin, heatduty);
}
