#include "StepperMotorController.h"

StepperMotorController::StepperMotorController(int dirPin, int stepPin, int limLow, int limHigh)
  : directionPin(dirPin), stepPin(stepPin), limitLowPin(limLow), limitHighPin(limHigh) {}

void StepperMotorController::begin() {
  pinMode(directionPin, OUTPUT);
  pinMode(stepPin, OUTPUT);
  pinMode(limitLowPin, INPUT_PULLUP);
  pinMode(limitHighPin, INPUT_PULLUP);
}

void StepperMotorController::moveStepper() {
  digitalWrite(stepPin, HIGH);
  delayMicroseconds(1000);
  digitalWrite(stepPin, LOW);
  delayMicroseconds(1000);
}

void StepperMotorController::setDirection(bool dir) {
  digitalWrite(directionPin, dir ? HIGH : LOW);
}

int StepperMotorController::checkLimits() {
  if(digitalRead(limitHighPin) == HIGH) return 1;
  if(digitalRead(limitLowPin) == HIGH) return 2;
  return 0; 
}

void StepperMotorController::homeMotor() {
  if (motorsHomed) return;

  //Serial.println("Starting homing...");
  setDirection(true);
  while (checkLimits() == 0) {
    //Serial.print("High: ");
    //Serial.print(digitalRead(limitHighPin));
    //Serial.print(" | Low: ");
    //Serial.println(digitalRead(limitLowPin));
    moveStepper();
    delay(5);
  }
  //Serial.println("Hit first limit");

  setDirection(false);
  int currentLim = checkLimits();
  while (checkLimits() == currentLim) {
    moveStepper();
    delay(5);
    position++;
  }

  while (checkLimits() == 0) {
    moveStepper();
    delay(5);
    position++;
  }

  home = position / 10;
  setDirection(true);
  while (position > home) {
    moveStepper();
    delay(5);
    position--;
  }

  motorsHomed = true;
  Serial.println("Home set");
}

int StepperMotorController::getPosition() const {
  return position; 
}