extern const int outlet1     = 6;
extern const int pumpRelay   = 7;
extern const int lcdP1       = 8;
extern const int lcdP2       = 9;
extern const int lcdP3       = 10;
extern const int lcdP4       = 11;
extern const int lcdP5       = 12;
extern const int lcdP6       = 13;
extern const int buttonRight = 59;     
extern const int buttonLeft  = 3;    
extern const int buttonUp    = 2;    
extern const int buttonDown  = 58;    
extern const int alarm       = 4;    

void setupPins()
{
  pinMode(outlet1, OUTPUT);
  pinMode(pumpRelay,OUTPUT);
  pinMode(alarm, OUTPUT);
  pinMode(buttonRight, INPUT);
  pinMode(buttonLeft, INPUT);
  pinMode(buttonUp, INPUT);
  pinMode(buttonDown, INPUT); 
}

boolean isHeaterOn()
{
  if(digitalRead(outlet1) == HIGH)
  {
   return true; 
  }
  return false;
}

void turnAlarmOn()
{
  digitalWrite(alarm, HIGH); 
}

void turnAlarmOff()
{
  digitalWrite(alarm, LOW); 
}

boolean isAlarmOn()
{
  if(digitalRead(alarm) == HIGH)
  {
     return true;
  } 
  return false;
}

void turnHeaterOff()
{
 digitalWrite(outlet1, LOW);
}
