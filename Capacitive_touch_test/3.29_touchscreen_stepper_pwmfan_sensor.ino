
#include <UTFT.h> 
#include <Wire.h>

// Declare fonts
extern uint8_t GroteskBold16x32[];

// LCD setup
UTFT myGLCD(NT35510_16, 7, 38, 9, 10);

// Capacitive touch variables
uint8_t addr = 0x38;
#define FT6236_INT 5

//stepper variables
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
int pwm_fan_pin = 44;
float dutyCycle = 20.0;

// UI Variables
int windSpeed = 0;   
int temperature = 0;  
float currWindSpeed = 0;
int currTemperature = 0;

// Temp and Wind Sensor Pins
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

  pinMode(pwm_fan_pin, OUTPUT);
  TCCR5A = 0;
  TCCR5B = 0;
  TCNT5 = 0;

  TCCR5A = (1 << COM5C1) | (1 << WGM51);  // Clear OC5C on compare match, Fast PWM
  TCCR5B = (1 << WGM53) | (1 << WGM52) | (1 << CS50); // WGM=14, no prescaler

  ICR5 = 3199;   // Sets PWM frequency to 5kHz
  setDutyCycle(dutyCycle);  // Set desired duty cycle

}

void setDutyCycle(float percent) {
  percent = constrain(percent, 0, 100);
  OCR5C = (uint16_t)((percent / 100.0) * ICR5);
}

// Add this global variable at the top
unsigned long lastUpdateTime = 0;  // Track last update time
const unsigned long updateInterval = 1000;  // 500ms update interval

void loop() {
  if(Motorshomed == false)
{
  Direction(directionpin1, 1);
  while(Checklimits(limswitch1, limswitch2) == 0)
  {
    Step(stepper1);
    delay(5);
    Serial.println("step");
    Checklimits(limswitch1, limswitch2);
  }
  Direction(directionpin1, 0);
  currentlim = Checklimits(limswitch1, limswitch2);
  while(Checklimits(limswitch1, limswitch2) == currentlim)
  {
    Step(stepper1);
    delay(5);
    Serial.println("step");
    position++;
  }
  while(Checklimits(limswitch1, limswitch2) == 0)
  {
    Step(stepper1);
    delay(5);
    Serial.println("step");
    Checklimits(limswitch1, limswitch2);
    position++;
  }
  lowlim1 = Checklimits(limswitch1, limswitch2);
  home = position/2;
  Motorshomed = true;
  Direction(directionpin1, 1);
  while(position != home)
  {
    Step(stepper1);
    delay(5);
    Serial.println("step");
    position--;
  } 
  Serial.println("Home set");
}

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
      changeWindSpeed();  // 👈 update PWM based on new value
    } 
    else if (touchX > 330 && touchX < 380 && touchY > 500 && touchY < 560) {
      if (windSpeed > 0) { windSpeed--; }
      updateWindSpeedDisplay();
      changeWindSpeed();  // 👈 update PWM based on new value
      
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


void changeWindSpeed() {
  // Map windSpeed (0–400) to duty cycle (0–100%)
  float duty = map(windSpeed, 0, 400, 0, 100);
  setDutyCycle(duty);
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


//requires dirpin is direction pin, needs to be set as output
//dir = 0 = foward, dir = 1 backawards
void Direction(int dirpin, int dir)
{
    pinMode(dirpin, OUTPUT);
    if(dir == 0)
    {
      digitalWrite(dirpin, LOW);
    }
    if(dir == 1)
    {
      digitalWrite(dirpin, HIGH);
    }
}
//requires step pin = pin of the step, needs to be set as output
void Step(int steppin)
{
  pinMode(steppin, OUTPUT);
  digitalWrite(steppin,HIGH); //Trigger one step forward
  delay(1);
  digitalWrite(steppin,LOW); //Pull step pin low so it can be triggered again
  delay(1);
}
//Send pins for motor limits
//returns 1 for high lim actviated
//returns 2 for low lim activiated
int Checklimits(int highlim, int lowlim)
{
  pinMode(highlim, INPUT);
  pinMode(lowlim, INPUT);
  if(digitalRead(highlim) == HIGH)
  {
    Serial.println("Hit high lim"); 
    return 1;
  }
  if(digitalRead(lowlim) == HIGH)
  {
    Serial.println("Hit low lim"); 
    return 2;
  } 
    return 0;
}

