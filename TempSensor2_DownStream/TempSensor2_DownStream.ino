// Sensor analog input pins
const int analogPinForTMP = A0;  // Thermistor input
const int analogPinForRV  = A1;  // Wind sensor input

// Variables for sensor readings
int TMP_Therm_ADunits = 0;
int RV_Wind_ADunits   = 0;
float RV_Wind_Volts   = 0.0;
float zeroWind_ADunits = 0.0;
float zeroWind_volts  = 0.0;
float TempCtimes100   = 0.0;
float WindSpeed_MPH   = 0.0;
float ActualWindSpeed_MPH = 0.0;
float WindSpeed       = 0.0;
float Wind;
// Timing
unsigned long lastMillis = 0;

// Wind averaging buffer
const int bufferSize = 10;
float windSpeedBuffer[bufferSize];
int bufferIndex = 0;
int bufferCount = 0;
float windSpeedSum = 0.0;

// Constants
const float zeroWindAdjustment = 0.2;

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("Wind Speed and Temperature Sensor with Averaging");
}

void loop() {
  if (millis() - lastMillis > 200) {
    lastMillis = millis();

    // Read sensors
    TMP_Therm_ADunits = analogRead(analogPinForTMP);
    RV_Wind_ADunits   = analogRead(analogPinForRV);
    RV_Wind_Volts     = RV_Wind_ADunits * 0.0048828125;

    // Compute temperature
    TempCtimes100 = (0.005 * TMP_Therm_ADunits * TMP_Therm_ADunits) -
                    (16.862 * TMP_Therm_ADunits) + 9075.4;

    // Zero-wind baseline
    zeroWind_ADunits = -0.0006 * TMP_Therm_ADunits * TMP_Therm_ADunits +
                        1.0727 * TMP_Therm_ADunits + 47.172;
    zeroWind_volts = (zeroWind_ADunits * 0.0048828125) - zeroWindAdjustment;

    // Wind speed calculation
    WindSpeed_MPH = pow(((RV_Wind_Volts - zeroWind_volts) / 0.2300), 2.7265);
    ActualWindSpeed_MPH = 0.0511 * WindSpeed_MPH + 0.64;
    WindSpeed = 0.95 * ActualWindSpeed_MPH - 0.2;
    
    // Update circular buffer and compute running average
    windSpeedSum -= windSpeedBuffer[bufferIndex];  // remove oldest
    windSpeedBuffer[bufferIndex] = WindSpeed;      // insert newest
    windSpeedSum += WindSpeed;                     // add newest

    bufferIndex = (bufferIndex + 1) % bufferSize;
    if (bufferCount < bufferSize) bufferCount++;
    
    float windSpeedAvg = windSpeedSum / bufferCount;
    if (WindSpeed < 8) {
      WindSpeed = 0.9 * WindSpeed; 
    }
    // Output
    Serial.print("Temp (°C): ");
    Serial.print(TempCtimes100 / 100.0);
    Serial.print(" | Instant Wind Speed (MPH): ");
    Serial.print(WindSpeed);
    Serial.print(" | Mean of last ");
    Serial.print(bufferCount);
    Serial.print(" readings: ");
    Serial.println(windSpeedAvg);
  }
}
