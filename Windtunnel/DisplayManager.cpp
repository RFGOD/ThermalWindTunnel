#include "DisplayManager.h"

DisplayManager::DisplayManager() {}

void DisplayManager::begin() {
  display.begin();
  display.setRotation(1);
  display.fillScreen(0x000000);
  display.setTextColor(0xFFFFFF);
  display.setTextSize(3);

  if (!touch.begin()) {
    Serial.println("Touch initialization failed.");
    while (1); // Halt execution if touch controller fails to initialize
  } else {
    Serial.println("Touch controller initialized.");
  }
}

void DisplayManager::showTemperatureLabel() {
  display.setCursor(70, 30);
  display.print("Temperature");
}

void DisplayManager::showWindSpeedLabel() {
  display.setCursor(50, 250);
  display.print("Wind Speed (MPH)");
}

void DisplayManager::showCurrentTemperatureLabel() {
  display.setCursor(450, 30);
  display.print("Current Temperature");
}

void DisplayManager::showCurrentWindSpeedLabel() {
  display.setCursor(450, 250);
  display.print("Current Wind Speed");
}

void DisplayManager::drawTemperatureControls() {
  // Draw "-" button

  display.setCursor(70, 115);
  display.setTextColor(0xFFFFFF);
  display.setTextSize(3);
  display.print("-");

  // Draw count in middle
  display.fillRoundRect(126, 100, 80, 50, 10, 0x222222);
  display.setCursor(156, 115);
  display.setTextColor(0xFFFFFF);
  display.setTextSize(3);
  display.print(tempValue);

  // Draw "+" button
  //display.fillRoundRect(250, 100, 50, 50, 10, 0x555555);
  display.setCursor(250, 115);
  display.setTextColor(0xFFFFFF);
  display.setTextSize(3);
  display.print("+");
}

void DisplayManager::drawWindSpeedControls() {

  // Draw "-" button
  display.setCursor(70, 335);
  display.setTextColor(0xFFFFFF);
  display.setTextSize(3);
  display.print("-");

  //Draw count in middle
  display.fillRoundRect(126, 320, 80, 50, 10, 0x222222);
  display.setCursor(156, 335);
  display.setTextColor(0xFFFFFF);
  display.setTextSize(3);
  display.print(windValue);

  // Draw "+" button
  display.setCursor(250, 335);
  display.setTextColor(0xFFFFFF);
  display.setTextSize(3);
  display.print("+");
}

void DisplayManager::handleTouch() {
  GDTpoint_t points[5];  // Array to hold touch points
  uint8_t contacts = touch.getTouchPoints(points);

  if (contacts > 0) {
    //Serial.print("Contacts detected: ");
    //Serial.println(contacts);

    for (uint8_t i = 0; i < contacts; i++) {
      int x = points[i].x;
      int y = points[i].y;

      // Print touch point coordinates for debugging
      //Serial.print("Touch ");
      //Serial.print(i + 1);
      //Serial.print(": X = ");
      //Serial.print(x);
      //Serial.print(", Y = ");
      //Serial.println(y);


      ////////////////////////////////
      //     Temperature buttons
      ////////////////////////////////
      // Check if touch is within the "-" button area (330, 380, 70, 120)
      if (x >= 250 && x <= 450 && y >= 10 && y <= 115) {
        //Serial.println("Touch on '-' button");
        if (tempValue > tempMin) {
          tempValue--; // Decrease temperature
          drawTemperatureControls(); // Redraw with updated value
        }
      }
      // Check if touch is within the "+" button area (250, 100, 50, 50)
      if (x >= 250 && x <= 465 && y >= 210 && y <= 370) {
        //Serial.println("Touch on '+' button");
        if (tempValue < tempMax) {
          tempValue++; // Increase temperature
          drawTemperatureControls(); // Redraw with updated value
        }
      }

    /////////////////////////////////////
    //        wind speed buttons
    ////////////////////////////////////
      if (x >= 5 && x <= 200 && y >= 5 && y <= 130) {
        Serial.println("Touch on '-' button");
        if (windValue > windMin) {
          windValue--; // Decrease temperature
          drawWindSpeedControls(); // Redraw with updated value
        }
      }
      // Check if touch is within the "+" button area (250, 100, 50, 50)
      if (x >= 5 && x <= 200 && y >= 210 && y <= 350) {
        Serial.println("Touch on '+' button");
        if (windValue < windMax) {
          windValue++; // Increase temperature
          drawWindSpeedControls(); // Redraw with updated value
        }
      }

    }
  }
}


int DisplayManager::getTempValue() {
  return tempValue;
}

void DisplayManager::setTempValue(int newTempValue) {
  tempValue = newTempValue;
  drawTemperatureControls(); // Redraw with the updated value
}

int DisplayManager::getWindValue() {
  return windValue;
}

void DisplayManager::setWindValue(int newWindValue) {
  windValue = newWindValue;
  drawWindSpeedControls(); // Redraw with the updated value
}

GigaDisplay_GFX* DisplayManager::getDisplay() {
  return &display;
}