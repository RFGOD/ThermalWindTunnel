// ===========================
//        LIBRARIES
// ===========================
#include <UTFT.h> 
#include <Wire.h>

// ===========================
//      LCD & TOUCH SETUP
// ===========================
extern uint8_t GroteskBold16x32[];
UTFT myGLCD(NT35510_16, 7, 38, 9, 10);
uint8_t addr = 0x38;
#define FT6236_INT 5

// ===========================
//     STEPPER MOTOR SETUP
// ===========================
char user_input;
boolean Motorshomed = false;
int position = 0;
int limswitch1 = 14;
int limswitch2 = 15;
int directionpin1 = 16;
int stepper1 = 17;
int home = 0;

// ===========================
//       FAN CONTROL
// ===========================
int pwm_fan_pin = 44;
float dutyCycle = 20.0;

// ===========================
//       UI VARIABLES
// ===========================
int windSpeed = 0;   
int temperature = 0;  
float currWindSpeed = 0;
int currTemperature = 0;

// ===========================
//       SENSOR PINS
// ===========================
int analogPinRV1 = A0;
int analogPinTMP1 = A1;

// ===========================
//      SCREEN SETTINGS
// ===========================
const int SCREEN_WIDTH = 480;
const int SCREEN_HEIGHT = 800;

// ===========================
//   TOUCH LOCATION STRUCT
// ===========================
struct TouchLocation {
  int x;
  int y;
};

// ===========================
//   Read FT6236 Touch Location
// ===========================
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


// ===========================
//   SETUP FUNCTION
// ===========================
void setup() {
  Wire.begin();
  myGLCD.InitLCD();
  myGLCD.setFont(GroteskBold16x32);
  pinMode(FT6236_INT, INPUT);
  Serial.begin(9600);
  drawInterface();
  
  pinMode(pwm_fan_pin, OUTPUT);
  setupPWM();
  setDutyCycle(dutyCycle);
}

// ===========================
//     MAIN LOOP FUNCTION
// ===========================
void loop() {
  if (!Motorshomed) {
    homeMotor();
  }
  if (!digitalRead(FT6236_INT)) {
    processTouch();
  }
  updateSensors();
}

// ===========================
//       UI FUNCTIONS
// ===========================

void drawButton(int x, int y, const char *label) {
  myGLCD.setFont(GroteskBold16x32);
  myGLCD.setColor(255, 255, 255);
  myGLCD.print(label, x, y);
}

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

void updateWindSpeedDisplay() {
  char buffer[4];
  sprintf(buffer, "%d", windSpeed);
  myGLCD.setFont(GroteskBold16x32);
  myGLCD.setColor(0, 0, 0);
  myGLCD.fillRect(160, 320, 300, 360);
  myGLCD.setColor(255, 255, 255);
  myGLCD.print(buffer, 150, 350);
}

void updateTemperatureDisplay() {
  char buffer[4];
  sprintf(buffer, "%d", temperature);
  myGLCD.setFont(GroteskBold16x32);
  myGLCD.setColor(0, 0, 0);
  myGLCD.fillRect(160, 70, 300, 110);
  myGLCD.setColor(255, 255, 255);
  myGLCD.print(buffer, 150, 100);
}

void updateCurrentWindSpeedDisplay() {
  char buffer[6];
  dtostrf(currWindSpeed, 4, 2, buffer);
  myGLCD.setFont(GroteskBold16x32);
  myGLCD.setColor(0, 0, 0);
  myGLCD.fillRect(600, 320, 750, 360);
  myGLCD.setColor(255, 255, 255);
  myGLCD.print(buffer, 600, 350);
}

void updateCurrentTemperatureDisplay() {
  char buffer[6];
  sprintf(buffer, "%d", currTemperature);
  myGLCD.setFont(GroteskBold16x32);
  myGLCD.setColor(0, 0, 0);
  myGLCD.fillRect(600, 70, 750, 110);
  myGLCD.setColor(255, 255, 255);
  myGLCD.print(buffer, 600, 100);
}

// ===========================
//    TOUCH PROCESSING
// ===========================
void processTouch() {
  TouchLocation touch;
  if (readFT6236TouchLocation(&touch, 1) > 0) {
    int touchX = touch.x;
    int touchY = touch.y;

    // Wind Speed Buttons
    if (touchX > 330 && touchX < 380 && touchY > 700 && touchY < 760) {
      if (windSpeed < 400) { windSpeed++; }
      updateWindSpeedDisplay();
      changeWindSpeed();  // 👈 update PWM based on new value
      adjustMotorToWindSpeed();
    } 
    else if (touchX > 330 && touchX < 380 && touchY > 500 && touchY < 560) {
      if (windSpeed > 0) { windSpeed--; }
      updateWindSpeedDisplay();
      changeWindSpeed();  // 👈 update PWM based on new value
      adjustMotorToWindSpeed();
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

// ===========================
//    MOTOR CONTROL
// ===========================

void homeMotor() {
  digitalWrite(directionpin1, HIGH); // HIGH means move forward
  while (checkLimits() == 0) {
    moveStepper(1, 1); // Move stepper forward
  }
  digitalWrite(directionpin1, LOW); // LOW means move backward
  int currentlim = checkLimits();
  while (checkLimits() == currentlim) {
    moveStepper(1, 0); // Move stepper backward
    position++;
  }
  while (checkLimits() == 0) {
    moveStepper(1, 0);
    position++;
  }
  home = position / 2;
  Motorshomed = true;
  digitalWrite(directionpin1, HIGH); // Move forward again to set home position
  while (position != home) {
    moveStepper(1, 1);
    position--;
  }
  Serial.println("Home set");
}

void moveStepper(int steps, int dir) {
  digitalWrite(directionpin1, dir);
  for (int i = 0; i < steps; i++) {
    digitalWrite(stepper1, HIGH);  // Assuming you're using a simple HIGH/LOW to step the motor
    delay(1);
    digitalWrite(stepper1, LOW);
    delay(1);
  }
}

int checkLimits() {
  pinMode(limswitch1, INPUT); //highlim
  pinMode(limswitch2, INPUT); //lowlim
  if (digitalRead(limswitch1) == HIGH) {
    Serial.println("Hit high lim");
    return 1;
  }
  if (digitalRead(limswitch2) == HIGH) {
    Serial.println("Hit low lim");
    return 2;
  }
  return 0;
}

/// Function to increase or decrease LFM with motor
void adjustMotorToWindSpeed() {
  int stepsToMove = abs(windSpeed - position);

  if (windSpeed > position) {
      moveStepper(stepsToMove, 1); // Move forward
      position += stepsToMove;
  } else if (windSpeed < position) {
      moveStepper(stepsToMove, 0); // Move backward
      position -= stepsToMove;
  }
}

// ===========================
//    SENSOR READING
// ===========================
void updateSensors() {
  int TMP1_Therm_ADunits = analogRead(analogPinTMP1);  
  float TMP1_Volts = TMP1_Therm_ADunits * 0.0048828125;
  int RV1_Wind_ADunits = analogRead(analogPinRV1);
  float RV1_Wind_Volts = RV1_Wind_ADunits * 0.0048828125;
  float zeroWind_ADunits = -0.0006 * (TMP1_Therm_ADunits * TMP1_Therm_ADunits) + 1.0727 * TMP1_Therm_ADunits + 47.172;
  float zeroWind_volts = zeroWind_ADunits * 0.0048828125;
  currTemperature = (0.00391 * (TMP1_Therm_ADunits * TMP1_Therm_ADunits)) - (14.862 * TMP1_Therm_ADunits) + 8575.4;
  currTemperature /= 100;
  currWindSpeed = (RV1_Wind_Volts < zeroWind_volts) ? 0 : pow(((RV1_Wind_Volts - zeroWind_volts) / 0.2300), 2.7265) * 88;
  updateCurrentTemperatureDisplay();
  updateCurrentWindSpeedDisplay();
}

// ===========================
//    PWM CONTROL FOR FAN
// ===========================
void setupPWM() {
  TCCR1A = (1 << COM1B1) | (1 << WGM11);  // Adjusted to work with 16-bit timers (Timer 1)
  TCCR1B = (1 << WGM13) | (1 << WGM12) | (1 << CS10);  // Adjusted to work with 16-bit timers (Timer 1)
  ICR1 = 3199;  // Define the frequency of the PWM signal
}

void changeWindSpeed() {
  // Map the windSpeed value to a PWM duty cycle
  dutyCycle = map(windSpeed, 0, 400, 0, 100);  // Maps windSpeed from 0 to 400 to a duty cycle of 0% to 100%
  setDutyCycle(dutyCycle);  // Update PWM based on the mapped duty cycle
}

void setDutyCycle(float duty) {
  duty = constrain(duty, 0, 100);
  OCR1B = (uint16_t)((duty / 100.0) * ICR1);  // Set PWM duty cycle
}
