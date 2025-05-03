#include "DisplayManager.h"
#include "StepperMotorController.h"
#include "FanController.h"
#include "SensorManager.h"
#include "HeatController.h"

// === Analog Pins for Sensors ===
#define TEMP1_SENSOR_PIN A0 // down stream tmp
#define WIND1_SENSOR_PIN A1
#define TEMP2_SENSOR_PIN A2 // up stream tmp
#define WIND2_SENSOR_PIN A3

// === Global Components ===
FanController fan(12); // fan PWM Pin
HeatController heat(13); // heat PWM Pin
StepperMotorController motor2(6, 7, 8, 9); // dirPin, stepPin, lowLim, highLim
StepperMotorController motor1(2, 3, 4, 5); // Heater motor
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

  // heat setup
  heat.begin();

}

void loop() {
  // Handle touch input continuously
  screen.handleTouch();
  
   // Update fan based on UI wind setting
  int currentWind = screen.getWindValue();
  fan.setWindSpeed(currentWind);

  int currentHeat = screen.getTempValue();
  heat.setHeatSpeed(currentHeat);


  // Debug output
  //Serial.print("Wind Value: ");
  //Serial.println(currentWind);

  // Update sensors and display readings
  sensor.updateSensors();

  delay(100); // Moderate refresh rate
}
