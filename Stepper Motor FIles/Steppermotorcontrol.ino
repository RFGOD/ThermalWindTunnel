char user_input;

void setup() {
    Serial.begin(9600); //Open Serial connection for debugging
}

void loop() {
}

//requires dirpin is direction pin, needs to be set as output
//dir = 0 = foward, dir = 2 backawards
void Directtion(int dirpin, int dir)
{
    pinMode(dirpin, OUTPUT);
    if(dir = 0)
    {
      digitalWrite(dirpin, LOW);
    }
    if(dir = 1)
    {
      digitalWrite(dirpin, LOW);
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
void Checklimits(int highlim, int lowlim)
{
  pinMode(highlim, INPUT);
  pinMode(lowlim, INPUT);
  if(digitalRead(highlim) == LOW)
  {
    return 1;
  }
  if(digitalRead(lowlim) == LOW)
  {
    return 2;
  }
    return 0;
}
