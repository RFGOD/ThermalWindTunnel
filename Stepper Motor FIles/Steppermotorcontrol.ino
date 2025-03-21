char user_input;
boolean Motorshomed = false;
int position = 0;
int limswitch1 = 8;
int limswitch2 = 9;
int lowlim1 = 0;
int directionpin1 = 3;
int stepper1 = 2;
int currentlim = 5;
int home = 0;
void setup() {
    Serial.begin(9600); //Open Serial connection for debugging
}

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
