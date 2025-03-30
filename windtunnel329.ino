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
int lowlim1 = 0;
int directionpin1 = 16;
int stepper1 = 17;
int currentlim = 5;
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

unsigned long lastUpdateTime = 0;  // Track last update time
const unsigned long updateInterval = 1000;  // 500ms update interval

void loop() {
    homeMotor();

  if (!digitalRead(FT6236_INT)) {
    processTouch();
  }
  // ** Read sensor values and update UI only at intervals **
  if (millis() - lastUpdateTime >= updateInterval) {
    lastUpdateTime = millis();  // Update the last time we refreshed data
    updateSensors();
  }
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
      //adjustMotorToWindSpeed();
    } 
    else if (touchX > 330 && touchX < 380 && touchY > 500 && touchY < 560) {
      if (windSpeed > 0) { windSpeed--; }
      updateWindSpeedDisplay();
      changeWindSpeed();  // 👈 update PWM based on new value
      //adjustMotorToWindSpeed();
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
  if (Motorshomed == false) {
  Direction(directionpin1, 1); // HIGH means move forward
  while (checkLimits(limswitch1, limswitch2) == 0) 
  {
    moveStepper(stepper1); // Move stepper forward
    delay(5);
    Serial.println("step");
    checkLimits(limswitch1, limswitch2);
  }
  Direction(directionpin1, 0);
  currentlim = checkLimits(limswitch1, limswitch2);
  while(checkLimits(limswitch1, limswitch2) == currentlim) 
  {
    moveStepper(stepper1);
    delay(5);
    Serial.println("step");
    position++;
  }
  while(checkLimits(limswitch1, limswitch2) == 0) 
  {
    moveStepper(stepper1);
    delay(5);
    Serial.println("step");
    checkLimits(limswitch1, limswitch2);
    position++;
  }
  lowlim1 = checkLimits(limswitch1, limswitch2);
  home = position / 2;
  Motorshomed = true;
  Direction(directionpin1, 1); // Move forward again to set home position
  while (position != home) 
  {
    moveStepper(stepper1);
    delay(5);
    Serial.println("step");
    position--;
  }
  Serial.println("Home set");
  }
}
void moveStepper(int steppin) 
{
  pinMode(steppin, OUTPUT);
  digitalWrite(steppin,HIGH); //Trigger one step forward
  delay(1);
  digitalWrite(steppin,LOW); //Pull step pin low so it can be triggered again
  delay(1);
}

int checkLimits(int highLim, int lowLim) {
  pinMode(highLim, INPUT); //highlim
  pinMode(lowLim, INPUT); //lowlim

  if (digitalRead(highLim) == HIGH) {
    Serial.println("Hit high lim");
    return 1;
  }
  if (digitalRead(lowLim) == HIGH) {
    Serial.println("Hit low lim");
    return 2;
  }
  return 0;
}

//Direction function of motor 
//requires dirpin is direction pin, needs to be set as output
//dir = 0 = foward, dir = 1 backawards
void Direction(int dirPin, int dir)
{
    pinMode(dirPin, OUTPUT);
    if(dir == 0)
    {
      digitalWrite(dirPin, LOW);
    }
    if(dir == 1)
    {
      digitalWrite(dirPin, HIGH);
    }
}


/// Function to increase or decrease LFM with motor
/*void adjustMotorToWindSpeed() {
  int stepsToMove = abs(windSpeed - position);

  if (windSpeed > position) {
      moveStepper(stepsToMove, 1); // Move forward
      position += stepsToMove;
  } else if (windSpeed < position) {
      moveStepper(stepsToMove, 0); // Move backward
      position -= stepsToMove;
  }
} 
*/
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

  //Convert Temp to °C with improved accuracy
  currTemperature = (0.00391 * (TMP1_Therm_ADunits * TMP1_Therm_ADunits)) - (14.862 * TMP1_Therm_ADunits) + 8575.4;
  currTemperature /= 100; // Convert from times 100 format

  // Calculate Wind Speed in LFM (Linear Feet per Minute)
  if (RV1_Wind_Volts < zeroWind_volts) {
    currWindSpeed = 0;  
  } else {
    currWindSpeed = pow(((RV1_Wind_Volts - zeroWind_volts) / 0.2300), 2.7265) * 88; // Convert mph to LFM
  }

  updateCurrentWindSpeedDisplay();
  updateCurrentTemperatureDisplay();
}

// ===========================
//    PWM CONTROL FOR FAN
// ===========================
void setupPWM() {
  TCCR5A = 0;
  TCCR5B = 0;
  TCNT5 = 0;

  TCCR5A = (1 << COM5C1) | (1 << WGM51);  // Clear OC5C on compare match, Fast PWM
  TCCR5B = (1 << WGM53) | (1 << WGM52) | (1 << CS50); // WGM=14, no prescaler

  ICR5 = 3199;   // Sets PWM frequency to 5kHz
}

void changeWindSpeed() {
  // Map the windSpeed value to a PWM duty cycle
  float duty = map(windSpeed, 0, 400, 0, 100);  // Maps windSpeed from 0 to 400 to a duty cycle of 0% to 100%
  setDutyCycle(duty);  // Update PWM based on the mapped duty cycle
}

void setDutyCycle(float percent) {
  percent = constrain(percent, 0, 100);
  OCR5C = (uint16_t)((percent / 100.0) * ICR5);  // Set PWM duty cycle
}
