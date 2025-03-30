//RV  Airflow
int analogPinRV1 = A0;  // Analog pin for Air-Flow
int analogValueRV1 = 0;  // Variable to store the reading
float voltageRV1 = 0.0;  // Variable to store the voltage
float RV1_Wind_ADunits; // RV output from wind sensor
float RV1_Wind_Volts;

//TMP Temperature
int analogPinTMP1 = A1;  // Pin where the voltage is connected
int analogValueTMP1 = 0;  // Variable to store the reading
float voltageTMP1 = 0.0;  // Variable to store the voltage
int TMP1_Therm_ADunits; //Temperature termistor value from wind sensor

int TempCtimes100;
float zeroWind_ADunits;
float zeroWind_volts;
float WindSpeed_MPH;

void setup() {
  Serial.begin(9600);  // Initialize serial communication
}

void loop() {
  RV1_Wind_ADunits = analogRead(analogPinRV1);  // Read the analog input for Airflow for first senor
  RV1_Wind_Volts = (RV1_Wind_ADunits * 0.0048828125); // Convert data to Voltage
  TMP1_Therm_ADunits = analogRead(analogPinTMP1);  // Read the analog input for Temperature for first senor

  TempCtimes100 = (0.005 *((float)TMP1_Therm_ADunits * (float)TMP1_Therm_ADunits)) - (16.862 * (float)TMP1_Therm_ADunits) + 9075.4;
  
  zeroWind_ADunits = -0.0006*((float)TMP1_Therm_ADunits * (float)TMP1_Therm_ADunits) + 1.0727 * (float)TMP1_Therm_ADunits + 47.172;

  zeroWind_volts = (zeroWind_ADunits * 0.0048828125);

  if (RV1_Wind_Volts < zeroWind_volts) {
     WindSpeed_MPH = 0;  // No negative wind speed
   } else {
     WindSpeed_MPH = pow(((RV1_Wind_Volts - zeroWind_volts) / 0.2300), 2.7265);
   }

  Serial.print(" TMP volts ");
  Serial.print(TMP1_Therm_ADunits * 0.0048828125);

  Serial.print(" RV volts ");
  Serial.print((float)RV1_Wind_Volts);

  Serial.print("\t  TempC*100 ");
  Serial.print(TempCtimes100 );

  Serial.print("   ZeroWind volts ");
  Serial.print(zeroWind_volts);

  Serial.print("   WindSpeed MPH ");
  Serial.println((float)WindSpeed_MPH);

  delay(1000);
}