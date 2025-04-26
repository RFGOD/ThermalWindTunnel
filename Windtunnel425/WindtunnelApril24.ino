#include "DisplayManager.h"
#include "StepperMotorController.h"
#include "FanController.h"
#include "SensorManager.h"


// === Analog Pins for Sensors ===
#define TEMP1_SENSOR_PIN A0
#define WIND1_SENSOR_PIN A1
#define TEMP2_SENSOR_PIN A2
#define WIND2_SENSOR_PIN A3

// === Global Components ===
FanController fan(13); // PWM Pin
StepperMotorController motor1(6, 7, 8, 9); // dirPin, stepPin, lowLim, highLim
StepperMotorController motor2(2, 3, 4, 5); // Heater motor
DisplayManager screen;  
SensorManager sensor(A0, A1, A2, A3, &screen);  // Passing the pointer to 'screen'


void setup() {
  Serial.begin(115200);

  // Stepper motor setup
  motor1.begin();
  motor2.begin();
  motor1.homeMotor();
  //delay(100000);
  motor2.homeMotor();
  //delay(100000);

  // UI + Display
  screen.begin();
  //delay(3000); 
  screen.showTemperatureLabel();
  screen.showWindSpeedLabel();
  screen.showCurrentTemperatureLabel();
  screen.showCurrentWindSpeedLabel();
  screen.drawTemperatureControls();
  screen.drawWindSpeedControls();

  // Fan setup
  fan.begin();
}

void loop() {
  // Handle touch input continuously
  screen.handleTouch();
  
   // Update fan based on UI wind setting
  int currentWind = screen.getWindValue();
  fan.setWindSpeed(currentWind);

  // Debug output
  //Serial.print("Wind Value: ");
  //Serial.println(currentWind);

  // Update sensors and display readings
  sensor.updateSensors();

  delay(100); // Moderate refresh rate
}
