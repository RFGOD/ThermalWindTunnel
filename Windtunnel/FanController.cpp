#include "FanController.h"

FanController::FanController(int pwmPin) {
  _pwmPin = pwmPin;
  _windSpeed = 0;
}

void FanController::begin() {
  pinMode(_pwmPin, OUTPUT);
  analogWriteResolution(16); // 16-bit PWM
  analogWrite(_pwmPin, 0);  // Start fan off
}

void FanController::setWindSpeed(int speed) {
  _windSpeed = constrain(speed, 0, 100);
  updatePWM();
}

void FanController::updatePWM() {
  float duty = map(_windSpeed, 0, 100, 0, 65535); // analogWrite uses 0–65535 giga boads can support 16-bits
  analogWrite(_pwmPin, duty);
}
