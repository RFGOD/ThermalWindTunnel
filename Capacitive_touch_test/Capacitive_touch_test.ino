#include <UTFT.h> 
#include <Wire.h>

// Declare fonts
extern uint8_t GroteskBold16x32[];

// LCD setup
UTFT myGLCD(NT35510_16, 7, 38, 9, 10);

// Capacitive touch variables
uint8_t addr = 0x38;
#define FT6236_INT 5

// UI Variables
int windSpeed = 0;   
int temperature = 0;  
float currWindSpeed = 0;
int currTemperature = 0;

// Sensor Pins
int analogPinRV1 = A0;  // Airflow sensor pin
int analogPinTMP1 = A1; // Temperature sensor pin

// Screen resolution
const int SCREEN_WIDTH = 480;
const int SCREEN_HEIGHT = 800;

// Struct for Touch Location
struct TouchLocation {
  int x;
  int y;
};

// Read FT6236 Touch Location
uint8_t readFT6236TouchLocation(TouchLocation *pLoc, uint8_t num) {
  uint8_t retVal = 0;
  uint8_t status;

  Wire.beginTransmission(addr);
  Wire.write(0x02);
  Wire.endTransmission();
  Wire.requestFrom(addr, (uint8_t)1);

  if (Wire.available()) {
    status = Wire.read();
    if ((status & 0x0F) == 0) return 0; 
  } else {
    return 0;
  }

  uint8_t points = status & 0x0F;
  Wire.beginTransmission(addr);
  Wire.write(0x03);
  Wire.endTransmission();
  Wire.requestFrom(addr, (uint8_t)(points * 6));

  for (uint8_t i = 0; i < points && i < num; i++) {
    pLoc[i].x = (Wire.read() & 0x0F) << 8 | Wire.read();
    pLoc[i].y = (Wire.read() & 0x0F) << 8 | Wire.read();
    Wire.read();
    Wire.read();
  }
  retVal = points;
  return retVal;
}

// Setup function
void setup() {
  Wire.begin();
  myGLCD.InitLCD();
  myGLCD.setFont(GroteskBold16x32);
  pinMode(FT6236_INT, INPUT);
  Serial.begin(9600);

  drawInterface();
}

// Add this global variable at the top
unsigned long lastUpdateTime = 0;  // Track last update time
const unsigned long updateInterval = 1000;  // 500ms update interval

void loop() {
  if (!digitalRead(FT6236_INT)) {
    processTouch();
  }

  // ** Read sensor values and update UI only at intervals **
  if (millis() - lastUpdateTime >= updateInterval) {
    lastUpdateTime = millis();  // Update the last time we refreshed data
    readSensors();
  }
}



void drawButton(int x, int y, const char *label) {
  myGLCD.setFont(GroteskBold16x32);
  myGLCD.setColor(255, 255, 255);
  myGLCD.print(label, x, y);
}
// Draw UI
void drawInterface() {
  myGLCD.clrScr();

  // Labels
  myGLCD.setFont(GroteskBold16x32);
  myGLCD.setColor(255, 255, 255);
  myGLCD.print("Wind Speed LFM", 50, 250);
  myGLCD.print("Current Wind Speed LFM", 445, 250);
  myGLCD.print("Temperature C", 50, 0);
  myGLCD.print("Current Temperature C", 450, 0);
  
  // Update displayed values
  updateWindSpeedDisplay();
  updateCurrentWindSpeedDisplay();
  updateTemperatureDisplay();
  updateCurrentTemperatureDisplay();

  // Buttons
  drawButton(60, 350, "+");
  drawButton(260, 350, "-");
  drawButton(60, 100, "+");
  drawButton(260, 100, "-");
}

// Process Touch
void processTouch() {
  TouchLocation touch;
  if (readFT6236TouchLocation(&touch, 1) > 0) {
    int touchX = touch.x;
    int touchY = touch.y;

    // Wind Speed Buttons
    if (touchX > 330 && touchX < 380 && touchY > 700 && touchY < 760) {
      if (windSpeed < 400) { windSpeed++; }
      updateWindSpeedDisplay();
    } 
    else if (touchX > 330 && touchX < 380 && touchY > 500 && touchY < 560) {
      if (windSpeed > 0) { windSpeed--; }
      updateWindSpeedDisplay();
    }

    // Temperature Buttons
    if (touchX > 120 && touchX < 170 && touchY > 700 && touchY < 760) {     
      if (temperature < 90) { temperature++; }
      updateTemperatureDisplay();
    } 
    else if (touchX > 120 && touchX < 170 && touchY > 500 && touchY < 560) {
      if (temperature > 0) { temperature--; }
      updateTemperatureDisplay();
    }
  }
}

// Read Sensor Data
void readSensors() {
  int TMP1_Therm_ADunits = analogRead(analogPinTMP1);  
  float TMP1_Volts = TMP1_Therm_ADunits * 0.0048828125;

  int RV1_Wind_ADunits = analogRead(analogPinRV1);
  float RV1_Wind_Volts = RV1_Wind_ADunits * 0.0048828125;

  float zeroWind_ADunits = -0.0006 * (TMP1_Therm_ADunits * TMP1_Therm_ADunits) + 1.0727 * TMP1_Therm_ADunits + 47.172;
  float zeroWind_volts = zeroWind_ADunits * 0.0048828125;

  // Convert Temp to °C
  currTemperature = (0.005 * (TMP1_Therm_ADunits * TMP1_Therm_ADunits)) - (16.862 * TMP1_Therm_ADunits) + 9075.4;
  currTemperature /= 100;  // Convert from times 100 format

  // Calculate Wind Speed in MPH
  if (RV1_Wind_Volts < zeroWind_volts) {
    currWindSpeed = 0;  
  } else {
    currWindSpeed = pow(((RV1_Wind_Volts - zeroWind_volts) / 0.2300), 2.7265);
  }

  // Print for Debugging
  Serial.print("Current Temp (C): ");
  Serial.print(currTemperature);
  Serial.print("  |  Wind Speed (MPH): ");
  Serial.println(currWindSpeed);

  // Update UI
  updateCurrentTemperatureDisplay();
  updateCurrentWindSpeedDisplay();
}

// Update Wind Speed UI
void updateWindSpeedDisplay() {
  char buffer[4];
  sprintf(buffer, "%d", windSpeed);
  myGLCD.setFont(GroteskBold16x32);
  myGLCD.setColor(0, 0, 0);
  myGLCD.fillRect(160, 320, 300, 360);
  myGLCD.setColor(255, 255, 255);
  myGLCD.print(buffer, 150, 350);
}

// Update Temperature UI
void updateTemperatureDisplay() {
  char buffer[4];
  sprintf(buffer, "%d", temperature);
  myGLCD.setFont(GroteskBold16x32);
  myGLCD.setColor(0, 0, 0);
  myGLCD.fillRect(160, 70, 300, 110);
  myGLCD.setColor(255, 255, 255);
  myGLCD.print(buffer, 150, 100);
}

// Update Current Wind Speed UI
void updateCurrentWindSpeedDisplay() {
  char buffer[6];
  dtostrf(currWindSpeed, 4, 2, buffer);
  myGLCD.setFont(GroteskBold16x32);
  myGLCD.setColor(0, 0, 0);
  myGLCD.fillRect(600, 320, 750, 360);
  myGLCD.setColor(255, 255, 255);
  myGLCD.print(buffer, 600, 350);
}

// Update Current Temperature UI
void updateCurrentTemperatureDisplay() {
  char buffer[6];
  sprintf(buffer, "%d", currTemperature);
  myGLCD.setFont(GroteskBold16x32);
  myGLCD.setColor(0, 0, 0);
  myGLCD.fillRect(600, 70, 750, 110);
  myGLCD.setColor(255, 255, 255);
  myGLCD.print(buffer, 600, 100);
}
