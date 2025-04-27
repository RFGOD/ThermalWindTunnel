#ifndef SENSOR_MANAGER_H
#define SENSOR_MANAGER_H

#include <Arduino_GigaDisplay_GFX.h>
#include "DisplayManager.h"
#include <math.h>

class SensorManager {
public:
  SensorManager(int tmpPin1, int windPin1, int tmpPin2, int windPin2, DisplayManager* display);
  void updateSensors();
  float getTemperature() const;
  float getWindSpeed() const;
  float lastDisplayedTemp = -1000.0;
  float lastDisplayedWind = -1000.0;
private:
  void updateCurrentWindSpeedDisplay();
  void updateCurrentTemperatureDisplay();

  int analogPinTMP1;
  int analogPinRV1;
  float currTemperature = 0; 
  float currWindSpeed = 0;

  int analogPinTMP2;
  int analogPinRV2;
  float currTemperature2 = -999.0;
  float lastDisplayedTemp2 = -999.0;
  float currWindSpeed2 = 0;
  float lastDisplayTemp2 = 0;

  GigaDisplay_GFX* gfx;
};

#endif