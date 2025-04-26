#include "FanController.h"

FanController::FanController(int pwmPin) {
  _pwmPin = pwmPin;
  _windSpeed = 0;
}

void FanController::begin() {
  pinMode(_pwmPin, OUTPUT);
  analogWrite(_pwmPin, 0);  // Start fan off
}

void FanController::setWindSpeed(int speed) {
  _windSpeed = constrain(speed, 0, 100);
  updatePWM();
}

void FanController::updatePWM() {
  float duty = map(_windSpeed, 0, 100, 0, 400); // analogWrite uses 0–255
  analogWrite(_pwmPin, duty);
}
