#include "SensorManager.h"
#include <Arduino.h>
#include <stdlib.h>


SensorManager::SensorManager(int tmpPin1, int windPin1, int tmpPin2, int windPin2, DisplayManager* display)
  : analogPinTMP1(tmpPin1), analogPinRV1(windPin1),
    analogPinTMP2(tmpPin2), analogPinRV2(windPin2),
    gfx((GigaDisplay_GFX*)display) {}

void SensorManager::updateSensors() {
  int TMP1_Therm_ADunits = analogRead(analogPinTMP1); 
  float TMP1_Volts = TMP1_Therm_ADunits * (3.3 / 1023.0);  // Assuming 3.3V ADC reference

  int RV1_Wind_ADunits = analogRead(analogPinRV1);
  float RV1_Wind_Volts = RV1_Wind_ADunits * (3.3 / 1023.0);

  // --- Approximate Temperature (Celsius) ---
  currTemperature = 35.0 - ((TMP1_Volts - 1.5) / (2.4 - 1.5)) * 15.0;

  // Clamp temperature to detect out-of-range
  if (currTemperature > 50 || currTemperature < -20) {
    currTemperature = -999.0;  // Fault flag
  }

  // --- Wind Speed Calculation (Adjust for better scale) ---
  float zeroWind_Volts = 1.8;  // Refined zero-point voltage for no wind (adjust this to your sensor's behavior)
  
  if (RV1_Wind_Volts < zeroWind_Volts) {
    currWindSpeed = 0;
  } else {
    // Adjust this formula based on how the sensor behaves and calibrate as needed
    currWindSpeed = (RV1_Wind_Volts - zeroWind_Volts) * 500.0;  // Adjust scaling factor (lower than previous)
  }

  // Read second temperature
  int TMP2_ADunits = analogRead(analogPinTMP2);
  float TMP2_Volts = TMP2_ADunits * (3.3 / 1023.0);
  currTemperature2 = 35.0 - ((TMP2_Volts - 1.5) / (2.4 - 1.5)) * 15.0;
  if (currTemperature2 > 50 || currTemperature2 < -20) {
    currTemperature2 = -999.0;
  }

  // Read second wind
  int RV2_ADunits = analogRead(analogPinRV2);
  float RV2_Volts = RV2_ADunits * (3.3 / 1023.0);
  float zeroWind2 = 1.8;
  currWindSpeed2 = (RV2_Volts < zeroWind2) ? 0 : (RV2_Volts - zeroWind2) * 500.0;

  // --- Serial Debug Output ---
  Serial.print("TMP1_V: ");
  Serial.print(TMP1_Volts, 3);
  Serial.print(" | RV1_V: ");
  Serial.print(RV1_Wind_Volts, 3);
  Serial.print(" | Temp: ");
  Serial.print(currTemperature, 2);
  Serial.print(" | Wind km/h: ");
  Serial.println(currWindSpeed, 2);

  Serial.print("TMP2_V: ");
  Serial.print(TMP2_Volts, 3);
  Serial.print(" | RV2_V: ");
  Serial.print(RV2_Volts, 3);
  Serial.print(" | Temp: ");
  Serial.print(currTemperature2, 2);
  Serial.print(" | Wind km/h: ");
  Serial.println(currWindSpeed2, 2);

  // --- Display Update ---
  updateCurrentWindSpeedDisplay();
  updateCurrentTemperatureDisplay();
}




float SensorManager::getTemperature() const {
  return currTemperature;
}

float SensorManager::getWindSpeed() const {
  return currWindSpeed;
}

void SensorManager::updateCurrentWindSpeedDisplay() {
  if (abs(currWindSpeed - lastDisplayedWind) >= 0.1) {
    char buffer[10];
    snprintf(buffer, sizeof(buffer), "%.2f", lastDisplayedWind);  // Old value
    gfx->setTextSize(3);
    gfx->setTextColor(0x000000);  // Black text to "erase" old one
    gfx->setCursor(600, 330);
    gfx->print(buffer);

    lastDisplayedWind = currWindSpeed;
    snprintf(buffer, sizeof(buffer), "%.2f", currWindSpeed);  // New value
    gfx->setTextColor(0xFFFFFF);  // White text
    gfx->setCursor(600, 330);
    gfx->print(buffer);
  }
}

void SensorManager::updateCurrentTemperatureDisplay() {
  if (abs(currTemperature - lastDisplayedTemp) >= 0.1) {
    char buffer[10];
    snprintf(buffer, sizeof(buffer), "%.1f", lastDisplayedTemp);  // Old value
    gfx->setTextSize(3);
    gfx->setTextColor(0x000000);
    gfx->setCursor(600, 80);
    gfx->print(buffer);

    lastDisplayedTemp = currTemperature;
    snprintf(buffer, sizeof(buffer), "%.1f", currTemperature);  // New value
    gfx->setTextColor(0xFFFFFF);
    gfx->setCursor(600, 80);
    gfx->print(buffer);
  }
}

