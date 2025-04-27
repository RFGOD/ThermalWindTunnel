#include "HeatController.h"

void HeatController::HeatController(int heatPin) {
  _heatPin = heatPin;
  _heat = 0;
}

void HeatController::begin() {
  pinMode(_heatPin, OUTPUT);
  analogWriteResolution(16); // 16-bit PWM
  analogWrite(_heatPin, 0); // Start heat off
}

void HeatController::setHeat( int heat) {
  _heat = constrain(heat, 0, 100);
  updateHeatPWM();
}

void HeatController::updateHeatPWM() {
  float heatduty = map(_heat, 0, 100, 0, 65535); // analogWrite uses 0–65535 giga boads can support 16-bits
  analogWrite(_heat, heatduty);
}