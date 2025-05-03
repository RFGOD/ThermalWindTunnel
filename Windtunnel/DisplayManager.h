#ifndef DISPLAY_MANAGER_H
#define DISPLAY_MANAGER_H

#include <Arduino_GigaDisplay.h>
#include <Arduino_GigaDisplay_GFX.h>
#include <Wire.h> // For I2C communication
#include <Arduino_GigaDisplayTouch.h> // For touch functionality

class DisplayManager {
public:
  DisplayManager();
  void begin();
  void showTemperatureLabel();
  void showWindSpeedLabel();
  void showCurrentTemperatureLabel();
  void showCurrentWindSpeedLabel();

  void drawTemperatureControls(); // Modify this to use internal tempValue
  void handleTouch(); // Function to handle touch input
  int getTempValue(); // Getter for tempValue
  void setTempValue(int newTempValue); // Setter for tempValue

  void drawWindSpeedControls();
  int getWindValue();
  void setWindValue(int newWindValue);

  GigaDisplay_GFX* getDisplay();

private:
  GigaDisplay_GFX display;
  Arduino_GigaDisplayTouch touch;
  int tempValue = 1; // Initial Temperature value
  int tempMin = 0; // Minimum temperature
  int tempMax = 100; // Maximum temperature
  int windValue = 0;
  int windMin = 0;
  int windMax = 100; 
};

#endif
