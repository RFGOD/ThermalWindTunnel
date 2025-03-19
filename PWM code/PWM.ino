void setup() {
  pinMode(9, OUTPUT);  // Set pin 9 as output

  // Configure Timer1 for 5 kHz PWM
  TCCR1A = (1 << COM1A1) | (1 << WGM11);  // Fast PWM, non-inverting mode
  TCCR1B = (1 << WGM13) | (1 << WGM12) | (1 << CS11);  // Fast PWM, prescaler = 8

  ICR1 = 400;  // Set TOP value for 5 kHz

  OCR1A = 400;  // 50% duty cycle (Half of ICR1)
}

void loop() {
  // Nothing needed in loop, PWM runs automatically
}