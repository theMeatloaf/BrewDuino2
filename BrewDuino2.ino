#include <DallasTemperature.h>
extern const int outlet1, HLTTemp, pumpRelay;

//define brew cases...arbitrary
extern const int mash = 2;
extern const int doughin = 1;
extern const int mashout = 4;
extern const int wort = 3;
extern const int strike = 0;
extern const int emergencyShutoff = 5;
extern const int complete = 6;

#define STRIKE_HOLD_TIME 0 //seconds to hold strike after temp been reached...

//most important flag!
extern boolean readyToBrew = false;

static int brewStage = strike;//most important var for brew state machine
static int displayOption = 0;//which screen to display
unsigned int last = 0;//counter for display

void setup() {
  Serial.begin(9600);
  setupPins();
  setupLCD();
  startTempControl();
    
  //init input Screens
  populateScreenVars();

  //boot up screen display
  displayBootScreen();
  
  while(!readyToBrew)
  {
    //say in here unless its really ready to brew!
    inputRecipieLoop();
  }
  
  displayRecipieDebug();
      
  //start Brew
  changeScreens();
}

void loop() {
  if(resetScreen())
  {
   setupLCD(); 
  }
  
   if(emergencyShutdown())
  {
   //need to shutdown!
   turnAlarmOn();
   delay(2000);
   turnAlarmOff();
   brewStage = emergencyShutoff;
   last = 0;
  }
  
  //check for input here and change display case acordingly...also check for emergency
  int ButtonValue = Buttonloop(false,false);
  if(ButtonValue==3 || ButtonValue==4)
  { 
    if (displayOption != -1) {
      //we're not in a hold we can move along
      changeScreens();
      last = 0;
    
      if(ButtonValue == 3)displayOption--;
      else if(ButtonValue==4)
        displayOption++;
  
      //wrap around logic
      if(displayOption==4)
        displayOption = 0;
      if(displayOption==-1)
        displayOption = 3;
    }

  }
  
  switch(brewStage)
  {
    case strike:
      {
        strikeCase();
        break;
      }
  
    case mash:
      {
        mashCase();
        break;
      }

    case doughin:
      {
        doughinCase();
        break;
      }
  
    case mashout:
      {
        mashoutCase();
        break;
      }
  
    case wort:
      {
         wortCase();
         break;
      }
     case emergencyShutoff:
     {
        emergencyShutdownCase();
        break; 
     }
     case complete:
     {
       break; 
     }
  }
  
  if(!isAlarmOn())
  {
    //display screen as usual....
     if(getAllElapsed()>last)
      {
        //A second Has gone By...decide what to display
        switch(displayOption)
        {
        case 0:
          displaySpargeLCD();
          break;
    
        case 1:
          displayMashLCD();
          break;
    
        case 2:
          displayWortLCD();
          break;
    
        case 3:
          displayOverallLCD();
          break;

        default:
          //do nothing...
          //we are likely in a hold case
          break;
        }
        last = getAllElapsed();
      }
  }
}

//******EMERGENCY CASE BEGINS HERE**********//
void emergencyShutdownCase()
{
  turnHeaterOff();
}

//*********STRIKE CASE BEGINS HERE*********//
void strikeCase()
{
  //setup if just got here
  if(getHoldTemp() != getCurrentMashTemp())SetupHoldTemp(outlet1,getCurrentMashTemp(),4294967295);
  
  HoldTempDone(HLTTemp);

  //have we reached it and are just holding temp?
  if(strikeHoldEndTime() > 0)
  {
     if(getElapsed() > strikeHoldEndTime())
     {
       //been holding long enough...lets sound alarm and setup next temp
       int nextState = doughin;
       if (getNumberOfMashSteps() <= 1) {
          nextState = wort;
          displayOption = 2;
       }
       soundAlarm(true,nextState,"Strike Temp Reached!");
     } 
  }else if((getTempF(getTempNew(HLTTemp)) > (getHoldTemp()-10)) && getLastDismissedAlarmCount()==0 )
  {
      soundAlarm(true,strike,"Turn Off Stove!");
  }else if(getTempF(getTempNew(HLTTemp)) > getHoldTemp())
  {
        setupStrikeHoldTime(STRIKE_HOLD_TIME); //lets wait for some Time for good measure 
  }
}

//*********DOUGHIN CASE BEGINS HERE*********//
void doughinCase()
{
  //setup temp to zero to ensure it stays off while we do this
  if(getHoldTemp() != 0) {
    changeScreens();
    SetupHoldTemp(outlet1,0,4294967295);
  }
  HoldTempDone(HLTTemp);

  displayOption = -1;
  displayDoughInLCD();

  if(ButtonLoopLongPress(4))
  {
    //time to Move to Mash
    displayOption = 1;
    changeScreens();
    if(moveToNextMashStep())
    {
      setupMashCase();
      brewStage = mash;
    }
  }
}

//*********MASH CASE BEGINS HERE*********//
void mashCase() {
   if (HoldTempDone(HLTTemp))
   {
      //we're done holding mash time...
      //do we have more?
      if(moveToNextMashStep()) {
        //we do lets just set it up and keep in the mash
        setupMashCase();
      }
      else if (getLastDismissedAlarmCount()<1) 
      {
        //let the user know the mash is over with sound
        soundAlarm(true,mash,"Mash Complete!");
      } else {
        int ButtonValue = Buttonloop(true,false);
        if (ButtonValue == -1) {
              //move to mashout if they aren't pressing anything anymore
              brewStage = mashout;
        }
      }
   }
}

void setupMashCase()
{  
  //setup next mashStep temp control
  SetupHoldTemp(outlet1,getCurrentMashTemp(),getCurrentMashTime()); 

  //reset alarm for good measure
  resetAlarmCount();

  //turn on/off motor
  if(pumpIsOn())
  {
    digitalWrite(pumpRelay,HIGH);
  }
  else
  {
    digitalWrite(pumpRelay,LOW); 
  }
  last = 0; 
}

//***************MASHOUT CASE BEGINS HERE**************//
void mashoutCase() {  
   //setup temp to zero to ensure it stays off while we do this
  if(getHoldTemp() != 0) {
    changeScreens();
    SetupHoldTemp(outlet1,0,4294967295);
  }
  HoldTempDone(HLTTemp);

  displayOption = -1;
  displayMashOutInLCD();

  if(ButtonLoopLongPress(4))
  {
    //time to Move to Wort
    displayOption = 2;
    changeScreens();
    brewStage = wort;
  }
}

//***************WORT CASE BEGINS HERE**************//
void wortCase()
{   
      if(getHoldTemp() != wortGoalTemp())
      {
        //setup wort stuff if Just got here...including new PID vals
        setTempControlForBoil();
        SetupHoldTemp(outlet1,wortGoalTemp(),wortTotalTime());
        resetAlarmCount();
      }
  
      if(getTempF(getTempNew(HLTTemp)) < getHoldTemp()-3)
      {
       //check if its getting close so we can tell the user to pour in extract/turn off stove
       if(getTempF(getTempNew(HLTTemp)) > getHoldTemp()-5 && getLastDismissedAlarmCount()<1)
       {
        soundAlarm(true,wort,"Turn Off Stove/Pour In Excract");
       }
        
       // keep trying to hold temp and dont do anything else
       forceHoldTemp(HLTTemp);
       return;
      }
      else if(HoldTempDone(HLTTemp))
      {
       turnHeaterOff();
       soundAlarm(false, complete,"Brewing Complete!");////need to end here
       return;
      }else
      {
          if(incrementHopCounterIfShould())
          {
    
          }
          if     (hopInterval(0)<=getElapsed() && totalHopsSteps()>0 && getLastDismissedAlarmCount()<2){soundAlarm(true, wort,"Pour in Hops 1");}
          else if(hopInterval(1)<=getElapsed() && totalHopsSteps()>1 && getLastDismissedAlarmCount()<3){soundAlarm(true, wort,"Pour in Hops 2");}
          else if(hopInterval(2)<=getElapsed() && totalHopsSteps()>2 && getLastDismissedAlarmCount()<4){soundAlarm(true, wort,"Pour in Hops 3");}
      }
      

}

//**********Alarm Behaviors**********//
void soundAlarm(boolean tempHold, int nextCase, char message[ ])
{
  if(!isAlarmOn())changeScreens();//clear screen if this is first alarm
   turnAlarmOn();
   if(tempHold)forceHoldTemp(HLTTemp);else turnHeaterOff();
   displayAlarmMessage(message);
   int ButtonValue = Buttonloop(false,false);
   if(ButtonValue != -1)
   {
      //do Case Based Increases...
      if(currentBrewStage() == nextCase)dismissOneAlarm();
      changeScreens();
      turnAlarmOff();
      brewStage = nextCase;
   }
}

int currentBrewStage()
{
  return brewStage; 
}

boolean isBrewStage(int stage)
{
  if(stage == currentBrewStage() && !isAlarmOn())return true;
  else return false;
}
