#include <PID_v1.h>

double holdTemp, Input, Output;
int index = 0; // the index of the current reading

//Specify the links and initial tuning parameters
PID myPID(&Input, &Output, &holdTemp,172.12,0.14,0,DIRECT);

//size for Duty Cycle
int WindowSize = 1000;
unsigned long windowStartTime;

void startTempControl()
{
 windowStartTime = millis();

  setupTempSensor();

 //wait ot be sure getting accurate reading...
 delay(1000);

 //tell the PID to range between 0 and the full window size
 myPID.SetOutputLimits(0, WindowSize);

 //turn the PID on
 myPID.SetMode(AUTOMATIC);
}

void setTempControlForBoil() {
  myPID.SetTunings(452.71, 4.86, 0.0);
}


void PIDLoop(int pin,int vessel) {  
   float fTemp = getTempF(getTempNew(vessel));   

   if(!tempIsValid(fTemp))
   {
     //safety in case temp sensor is malfunctioning or isnt plugged in...
      digitalWrite(pin,LOW);
      return;
   }
    
    Input = fTemp; 

    myPID.Compute();
  
    unsigned long now = millis();
    if(now - windowStartTime>WindowSize)
    { 
      //time to shift the Relay Window
      windowStartTime += WindowSize;
    }
    if(Output >= now - windowStartTime)
    { 
      digitalWrite(pin,HIGH);
    }
    else 
    {
      digitalWrite(pin,LOW);
    }
}

double getHoldTemp()
{
   return holdTemp;
}

boolean tempIsValid(double temp )
{
  if(temp < 32 || temp > 300 )
   return false; 
  else
   return true;    
}

void tempControllerPrint()
{
    
  //Debug print
  Serial.print(" Goal Temp:");
  Serial.print(holdTemp);
  Serial.print(" Current Temp:");
  Serial.print(getTempF(getTempNew(HLTTemp)));
}
