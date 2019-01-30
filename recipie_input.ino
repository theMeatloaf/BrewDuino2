/*
  To Future Ryan: I've tagged things that need to be turned on for phase 2 by '#phase2' alrighty there big guy have a good one.
*/


#define maxScreenValues 18
#define sizeOfRecipieMem 142
#define numberOfSavedSlots 28

#define loadOrNewInputScreen 14
#define savedListPickingScreen 15
#define mashInputScreen 7
#define strikeInputScreen 10
#define wortInputScreen 12
#define saveStartQuestionScreen 13
#define saveNameInputScreen 16

#define integerVar 1
#define floatVar 2
#define on_offVar 3
#define yes_noVar 4
#define doneVar 6
#define leterVar 7

#define NUM_OF_MASH_INPUTS 7
#define NUM_OF_STRIKE_INPUTS 3
#define NUM_OF_WORT_INPUTS 14
#define NUM_OF_SAVE_START_INPUTS 2
#define NUM_OF_LOAD_NEW_INPUTS 2
#define NUM_OF_SAVED_LIST_INPUTS 4
#define NUM_OF_SAVED_NAME_INPUTS 18


#define FLASH_MILLIS 450

extern boolean readyToBrew;

//ints for flashing!
unsigned int lastFlashInt = millis()/FLASH_MILLIS;
unsigned int thisFlashInt;
int i;

//recipie struct to populate
static recipie inputRecipie;

//recipie temp vars
static int tempMashHours,tempMashMins,tempMashSecs;
static int tempMashIntervals;
static float tempMashTemp;
static float tempWortTemp = 200;
static boolean tempMashPumpOn,tempMoreMashSteps, tempSave, tempLoadOrNew, isCurSaving;
static int hopIntHours[3],hopIntMins[3],hopIntSecs[3];
static char tempName[18] = "                 ";

//name dictionary
static char possibleChars[] = {' ','a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z','1','2','3','4','5','6','7','8','9','*','!','$','#'};
 

//screen struct
struct screen
{
  int id;
  int locationRows[maxScreenValues];
  int locationCollums[maxScreenValues];
  //what each var type is....1 is int, 2 is float, 3 is off/on, 4 is yes/no, 5 is swap bool, 6 is DONE
  int varTypes[maxScreenValues];
  float * floatVars[maxScreenValues];
  int * intVars[maxScreenValues];
  boolean * onOffVars[maxScreenValues];
  boolean * yesNoVars[maxScreenValues];
  int varWidths[maxScreenValues];
  char * nameVars[17];
};

//screen and edit counters
static screen currentScreen;
static int curEdit=0;
static int curSavedDisplayPage = 0;
static int curDisplayedMashStep = 0;
static int curCharSelection = 0;

//screen defs
screen mashScreen,strikeScreen, wortScreen, saveQuestionScreen, loadOrNewScreen, savedListScreen, saveNameScreen;

//mash screen hardcoded vars
int mashRowLocations[NUM_OF_MASH_INPUTS] = {14,5,8,11,5,6,10};
int mashCollumnLocations[NUM_OF_MASH_INPUTS] = {0,1,1,1,2,3,3};
int mashVarTypes[NUM_OF_MASH_INPUTS] = {floatVar,integerVar,integerVar,integerVar,on_offVar,yes_noVar,doneVar};
int mashVarWidths[NUM_OF_MASH_INPUTS] = {6,2,2,2,3,3,5};
float * mashVarFloatVars[NUM_OF_MASH_INPUTS] = {&tempMashTemp,0,0,0,0,0,0};
int * mashVarIntVars[NUM_OF_MASH_INPUTS] = {0, &tempMashHours,&tempMashMins,&tempMashSecs,0,0,0};
boolean * mashOnOffVars[NUM_OF_MASH_INPUTS] = {0,0,0,0,&tempMashPumpOn,0,0};
boolean * mashYesNoVars[NUM_OF_MASH_INPUTS] = {0,0,0,0,0,&tempMoreMashSteps,0};

//strike screen hardcoded vars
int strikeRowLocations[NUM_OF_STRIKE_INPUTS] = {5,5,0};
int strikeCollumnLocations[NUM_OF_STRIKE_INPUTS] = {1,2,3};
int strikeVarTypes[NUM_OF_STRIKE_INPUTS] = {floatVar,on_offVar,doneVar};
int strikeVarWidths[NUM_OF_STRIKE_INPUTS] = {6,3,5};
float * strikeVarFloatVars[NUM_OF_STRIKE_INPUTS] = {&tempMashTemp,0,0};
boolean * strikeOnOffVars[NUM_OF_STRIKE_INPUTS] = {0,&tempMashPumpOn,0};

//wort screenvars
int wortRowsLocations[NUM_OF_WORT_INPUTS] = {5,12,15,18,5,8,11,5,8,11,5,8,11,14};
int wortCollumnLocations[NUM_OF_WORT_INPUTS] = {0,0,0,0,1,1,1,2,2,2,3,3,3,3};
int wortVarTypes[NUM_OF_WORT_INPUTS] = {floatVar,integerVar,integerVar,integerVar,integerVar,integerVar,integerVar,integerVar,integerVar,integerVar,integerVar,integerVar,integerVar,doneVar};
int wortVarWidths[NUM_OF_WORT_INPUTS] = {6,2,2,2,2,2,2,2,2,2,2,2,2,5};
float * wortVarFloatVars[NUM_OF_WORT_INPUTS] = {&tempWortTemp,0,0,0,0,0,0,0,0,0,0,0,0,0};
int * wortVarIntVars[NUM_OF_WORT_INPUTS] = {0,&tempMashHours,&tempMashMins,&tempMashSecs,&hopIntHours[0],&hopIntMins[0],&hopIntSecs[0],&hopIntHours[1],&hopIntMins[1],&hopIntSecs[1],&hopIntHours[2],&hopIntMins[2],&hopIntSecs[2],0};

//save/start screen vars
int saveStartRowLocations[NUM_OF_SAVE_START_INPUTS] = {12,8};
int saveStartCollumnLocaitons[NUM_OF_SAVE_START_INPUTS] = {1,2};
int saveStartVarTypes[NUM_OF_SAVE_START_INPUTS] = {yes_noVar,doneVar};
int saveStartVarWidths[NUM_OF_SAVE_START_INPUTS] = {3,6};
boolean * saveStartYesNoVars[NUM_OF_SAVE_START_INPUTS] = {&tempSave,0};

//load or new screen vars
int loadOrNewRowLocations[NUM_OF_LOAD_NEW_INPUTS] = {6,6};
int loadOrNewCollumnLocations[NUM_OF_LOAD_NEW_INPUTS] = {1,2};
int loadOrNewVarTypes[NUM_OF_LOAD_NEW_INPUTS] = {yes_noVar,doneVar};
int loadOrNewVarWidths[NUM_OF_LOAD_NEW_INPUTS] = {4,6};
boolean * loadOrNewYesNoVars[NUM_OF_LOAD_NEW_INPUTS] = {&tempLoadOrNew,0};

//saved List screen vars
int savedListRowLocations[NUM_OF_SAVED_LIST_INPUTS] = {3,3,3,3};
int savedListCollumnLocations[NUM_OF_SAVED_LIST_INPUTS] = {0,1,2,3};
int savedListVarWidths[NUM_OF_SAVED_LIST_INPUTS] = {17,17,17,17};
int savedListVarTypes[NUM_OF_SAVED_LIST_INPUTS] = {0,0,0,doneVar};

void populateScreenVars()
{   
  //mash defenitons
  mashScreen.id = mashInputScreen;
  for(i=0; i<NUM_OF_MASH_INPUTS; i++)
  {
     mashScreen.locationRows[i] = mashRowLocations[i];
     mashScreen.locationCollums[i] = mashCollumnLocations[i];
     mashScreen.varTypes[i] = mashVarTypes[i];
     mashScreen.varWidths[i] = mashVarWidths[i];
     mashScreen.floatVars[i] = mashVarFloatVars[i];
     mashScreen.intVars[i] = mashVarIntVars[i];
     mashScreen.onOffVars[i] = mashOnOffVars[i];
     mashScreen.yesNoVars[i] = mashYesNoVars[i];
  }
  
  strikeScreen.id = strikeInputScreen; 
  for(i=0; i<NUM_OF_STRIKE_INPUTS; i++)
  {
    strikeScreen.locationRows[i] = strikeRowLocations[i];
    strikeScreen.locationCollums[i] = strikeCollumnLocations[i];
    strikeScreen.varTypes[i] = strikeVarTypes[i];
    strikeScreen.varWidths[i] = strikeVarWidths[i];
    strikeScreen.floatVars[i] = strikeVarFloatVars[i];
    strikeScreen.onOffVars[i] = strikeOnOffVars[i];
  }

  wortScreen.id = wortInputScreen;
  for(i=0; i<NUM_OF_WORT_INPUTS; i++)
  {
     wortScreen.locationRows[i] = wortRowsLocations[i];
     wortScreen.locationCollums[i] = wortCollumnLocations[i];
     wortScreen.varTypes[i] = wortVarTypes[i];
     wortScreen.varWidths[i] = wortVarWidths[i];
     wortScreen.floatVars[i] = wortVarFloatVars[i];
     wortScreen.intVars[i] = wortVarIntVars[i];
  }
  
  saveQuestionScreen.id = saveStartQuestionScreen;
  for(i=0; i<NUM_OF_SAVE_START_INPUTS; i++)
  {
    saveQuestionScreen.locationRows[i] = saveStartRowLocations[i];
    saveQuestionScreen.locationCollums[i] = saveStartCollumnLocaitons[i];
    saveQuestionScreen.varTypes[i] = saveStartVarTypes[i];
    saveQuestionScreen.varWidths[i] = saveStartVarWidths[i];
    saveQuestionScreen.yesNoVars[i] =saveStartYesNoVars[i];
  }

  loadOrNewScreen.id = loadOrNewInputScreen;
  for(i=0; i<NUM_OF_LOAD_NEW_INPUTS; i++)
  {
    loadOrNewScreen.locationRows[i] = loadOrNewRowLocations[i];
    loadOrNewScreen.locationCollums[i] = loadOrNewCollumnLocations[i];
    loadOrNewScreen.varTypes[i] = loadOrNewVarTypes[i];
    loadOrNewScreen.varWidths[i] = loadOrNewVarWidths[i];
    loadOrNewScreen.yesNoVars[i] = loadOrNewYesNoVars[i];
  }
  
  savedListScreen.id = savedListPickingScreen;
  for(i=0; i<NUM_OF_SAVED_LIST_INPUTS; i++)
  {
   savedListScreen.locationRows[i] = savedListRowLocations[i];
   savedListScreen.locationCollums[i] = savedListCollumnLocations[i];
   savedListScreen.varWidths[i] = savedListVarWidths[i];
   savedListScreen.varTypes[i] = savedListVarTypes[i];
  }

  saveNameScreen.id = saveNameInputScreen;
  //populate name vars straight in loop since they are all the same but the last
  for(i=0; i<NUM_OF_SAVED_NAME_INPUTS-1; i++)
  {
    saveNameScreen.locationRows[i] = i;
    saveNameScreen.locationCollums[i] = 1;
    saveNameScreen.varWidths[i] = 1;
    saveNameScreen.varTypes[i] = leterVar;
    saveNameScreen.nameVars[i] = &tempName[i];
  }
  //do the "next" doneVar
  saveNameScreen.locationRows[NUM_OF_SAVED_NAME_INPUTS-1]=0;
  saveNameScreen.locationCollums[NUM_OF_SAVED_NAME_INPUTS-1]=2;
  saveNameScreen.varWidths[NUM_OF_SAVED_NAME_INPUTS-1]=5;
  saveNameScreen.varTypes[NUM_OF_SAVED_NAME_INPUTS-1]=doneVar;


  //initialize currentScreen
  currentScreen = loadOrNewScreen;
}


static boolean flashFlag = false;

void inputRecipieLoop()
{
  int ButtonValue = Buttonloop(true,true);
  if(ButtonValue != -1)
  {
    //code to refresh screen quickly
    thisFlashInt--;
    flashFlag=false;
  }
  switch(ButtonValue)
  { 
      case 2:
      {
       //down
       decreaseSelection();
       break;
      } 
      
      case 1:
      {
       //up
       increaseSelection();
       break;
      }
      
      case 3:
      {
        //left
        moveSelectionLeft();
        break;
      }
      
      case 4:
      {
        //right
        moveSelectionRight();
        break;
      }
  
  }
    
  lcd.setCursor(currentRow(),currentCollumn());
  if(flashFlag)
  {  
     for(i=0; i<curLength(); i++)
     {
      if(currentVarType()==leterVar && tempName[curEdit]==' ')
      {
       lcd.print("_"); 
      }else
      {
      lcd.print(" ");
      }
     }
  }else
  {
       printCurInputScreen();
  }
  
  
  if(thisFlashInt>lastFlashInt)
  {
    flashFlag = !flashFlag;
    lastFlashInt = thisFlashInt;
  }
  thisFlashInt = millis()/FLASH_MILLIS;

}

int currentVarType()
{
     return currentScreen.varTypes[curEdit];  
}

void printCurInputScreen()
{   
    switch(currentScreen.id)
    {
        case mashInputScreen:
        {
           lcd.setCursor(0,0);
           lcd.print("Mash #");
           if(curDisplayedMashStep < 10)lcd.print("0");
           lcd.print(curDisplayedMashStep);
           lcd.print(" Temp:");
           lcd.print(tempMashTemp);
           lcd.setCursor(0,1);
           lcd.print("TIME:");
           if(tempMashHours<10)lcd.print("0");
           lcd.print(tempMashHours);
           lcd.print(":");
           if(tempMashMins<10)lcd.print("0");
           lcd.print(tempMashMins);
           lcd.print(":");
           if(tempMashSecs<10)lcd.print("0");
           lcd.print(tempMashSecs);
           lcd.setCursor(0,2);
           lcd.print("Pump:");
           if(tempMashPumpOn)lcd.print("ON  ");
           else lcd.print("OFF ");
           
           lcd.setCursor(0,3);
           lcd.print("More?:");
           if(tempMoreMashSteps)lcd.print("YES");
           else lcd.print("NO ");
           lcd.print(" DONE?");
           break;
         } 
         case strikeInputScreen:
         {
           lcd.setCursor(6,0);
           lcd.print("STRIKE:");
           lcd.setCursor(0,1);
           lcd.print("TEMP:");
           lcd.print(tempMashTemp);
           lcd.setCursor(0,2);
           lcd.print("Pump:");
           lcd.print(tempMashPumpOn ? "YES" : "NO ");
           lcd.setCursor(0,3);
           lcd.print("DONE?");
           break;
         }
         case wortInputScreen:
         { 
          lcd.setCursor(0,0);
          lcd.print("TEMP:");
          lcd.print(tempWortTemp);
          lcd.print(" ");
          if(tempMashHours<10)lcd.print("0");
          lcd.print(tempMashHours);
          lcd.print(":");
          if(tempMashMins<10)lcd.print("0");
          lcd.print(tempMashMins);
          lcd.print(":");
          if(tempMashSecs<10)lcd.print("0");
          lcd.print(tempMashSecs);
          
          lcd.setCursor(0,1);
          lcd.print("HT1: ");
          if(hopIntHours[0]<10)lcd.print("0");
          lcd.print(hopIntHours[0]);
          lcd.print(":");
          if(hopIntMins[0]<10)lcd.print("0");
          lcd.print(hopIntMins[0]);
          lcd.print(":");
          if(hopIntSecs[0]<10)lcd.print("0");
          lcd.print(hopIntSecs[0]);
          
          lcd.setCursor(0,2);
          lcd.print("HT2: ");
          if(hopIntHours[1]<10)lcd.print("0");
          lcd.print(hopIntHours[1]);
          lcd.print(":");
          if(hopIntMins[1]<10)lcd.print("0");
          lcd.print(hopIntMins[1]);
          lcd.print(":");
          if(hopIntSecs[1]<10)lcd.print("0");
          lcd.print(hopIntSecs[1]);
          
          lcd.setCursor(0,3);
          lcd.print("HT3: ");
          if(hopIntHours[2]<10)lcd.print("0");
          lcd.print(hopIntHours[2]);
          lcd.print(":");
          if(hopIntMins[2]<10)lcd.print("0");
          lcd.print(hopIntMins[2]);
          lcd.print(":");
          if(hopIntSecs[2]<10)lcd.print("0");
          lcd.print(hopIntSecs[2]);
          
          lcd.print(" DONE?");
          break;
         }
         
         case saveStartQuestionScreen:
         {
          lcd.setCursor(3,0);
          lcd.print("RECIPIE ENTERED");
         
          lcd.setCursor(6,1);
          lcd.print("Save? ");
          
          if(tempSave)lcd.print("YES");
          else lcd.print("NO ");
          
          lcd.setCursor(8,2);
          if(tempSave)lcd.print("NEXT  ");
          else lcd.print("START!");
          
          break;
         }
         
         case loadOrNewInputScreen:
         {
          lcd.setCursor(0,0);
          lcd.print("Load Saved Or New?");
          
          lcd.setCursor(6,1);
          if(tempLoadOrNew)lcd.print("Load");
          else lcd.print("New ");
          
          lcd.setCursor(6,2);
          lcd.print("DONE?");
          break;
         }
         
         case savedListPickingScreen:
         {
           recipie tempRecipie;//this bug took 4 hours to find....I hate myself.
          for(i=0;i<4;i++)
          {
              lcd.setCursor(0,i);
              lcd.print((i+(curSavedDisplayPage*4)+1));
              if((i+(curSavedDisplayPage*4)+1)<10) lcd.print(" ");
              lcd.print(" ");
              EEPROM_readAnything(((i+(curSavedDisplayPage*4))*sizeOfRecipieMem),tempRecipie);
              if(tempRecipie.name[0]!='\0')lcd.print(tempRecipie.name);
              else lcd.print("empty");
          }
          break;
         }
         
         case saveNameInputScreen:
         {
           lcd.setCursor(0,0);
           lcd.print("NAME?:");
           lcd.setCursor(0,1);
           for(i=0; i<17; i++)lcd.print(tempName[i]);
           lcd.setCursor(0,2);
           lcd.print("NEXT?");
          break; 
         }
    }  
}

int currentRow()
{
    return currentScreen.locationRows[curEdit]; 
}

int currentCollumn()
{
    return currentScreen.locationCollums[curEdit];
}

int curLength()
{
      return currentScreen.varWidths[curEdit];
}

void moveSelectionRight()
{ 
    curCharSelection = 0;
  
    if(currentVarType() == doneVar)
    {
     //need to go to next Screen
        screenDone();
        return;
    }
   
  curEdit++;
}

void moveSelectionLeft()
{  
  if(curEdit==0)
  {
    screenBack();
    return;
  }
  curEdit--;
}

void increaseSelection()
{  
       //special case for saved list screen
       if(currentScreen.id == savedListPickingScreen)
       {
           //if saving...
           if(isCurSaving)
           {
            //save!
            
            EEPROM_writeAnything(((curEdit+(curSavedDisplayPage*4))*sizeOfRecipieMem),inputRecipie);
            lcd.clear();
            lcd.setCursor(8,1);
            lcd.print("SAVED!");
            delay(2000);
            isCurSaving = false;
            lcd.clear();
            currentScreen = savedListScreen;
           }else
           {
            EEPROM_readAnything(((curEdit+(curSavedDisplayPage*4))*sizeOfRecipieMem),inputRecipie);
            curDisplayedMashStep = 0;
            currentScreen = strikeScreen;
            lcd.clear();
            curEdit = 0;
            //set values on strike screen
            tempMashTemp = inputRecipie.mashTemps[0];
           }
          return;
       }
  
       if(currentVarType() == floatVar)
       {
         *currentScreen.floatVars[curEdit] = *currentScreen.floatVars[curEdit]+0.25;
       }
       if(currentVarType() == integerVar && *currentScreen.intVars[curEdit]<59)
       {
         *currentScreen.intVars[curEdit] = *currentScreen.intVars[curEdit]+1;
       }
       if(currentVarType() == on_offVar)
       {
          *currentScreen.onOffVars[curEdit] = !*currentScreen.onOffVars[curEdit];
       }
       if(currentVarType() == yes_noVar)
       {
         *currentScreen.yesNoVars[curEdit] = !*currentScreen.yesNoVars[curEdit];
       }
       if(currentVarType() == leterVar && curCharSelection < 39)
       {
         curCharSelection++;
         *currentScreen.nameVars[curEdit] =  possibleChars[curCharSelection];
       }
}

void decreaseSelection()
{ 
       if(currentVarType() == floatVar && *currentScreen.floatVars[curEdit]>0)
       {
         *currentScreen.floatVars[curEdit] = *currentScreen.floatVars[curEdit]-0.25;
       }
       if(currentVarType() == integerVar && *currentScreen.intVars[curEdit]>0)
       {
         *currentScreen.intVars[curEdit] = *currentScreen.intVars[curEdit]-1;
       }
       if(currentVarType() == on_offVar)
       {
          *currentScreen.onOffVars[curEdit] = !*currentScreen.onOffVars[curEdit];
       }
       if(currentVarType() == yes_noVar)
       {
         *currentScreen.yesNoVars[curEdit] = !*currentScreen.yesNoVars[curEdit];
       }
       if(currentVarType() == leterVar && curCharSelection > 0)
       {
         curCharSelection--;
         *currentScreen.nameVars[curEdit] =  possibleChars[curCharSelection];
       }
}

void screenDone()
{
 switch(currentScreen.id) 
  { 
   case saveNameInputScreen:
   {
     for(i=0; i<17; i++)
     {
       inputRecipie.name[i] = tempName[i];
     }
     isCurSaving = true;
     lcd.clear();
     curEdit = 0;
     curSavedDisplayPage = 0;
     currentScreen = savedListScreen;
     break;
   } 
    
   case savedListPickingScreen:
   {
     if(curSavedDisplayPage == ((numberOfSavedSlots/4)-1))
     {
      return;//dont go no where 
     }
     curSavedDisplayPage++;
     curEdit = 0;
     lcd.clear();
     break;
   } 
    
   case loadOrNewInputScreen:
   {
      if(tempLoadOrNew)
      {
        curEdit = 0;
        lcd.clear();
        currentScreen = savedListScreen;    
      }else
      {
       //start new blank recipie
       inputRecipie = emptyRecipie();
       tempMashTemp = inputRecipie.mashTemps[0];
       curEdit = 0;
       lcd.clear();
       currentScreen = strikeScreen; 
      }
      break; 
   }
    
   case strikeInputScreen:
   {
       //go to Mash screen
       inputRecipie.mashTemps[0] = tempMashTemp;
       inputRecipie.mashPumpStates[0] = false;
       inputRecipie.mashTimes[0] = 65534;//max Time
       if(inputRecipie.numberOfMashSteps==0)inputRecipie.numberOfMashSteps++;
       curDisplayedMashStep++;
       curEdit= 0;
       tempMashSecs = convertToDisSecs(inputRecipie.mashTimes[curDisplayedMashStep]);
       tempMashMins = convertToDisMins(inputRecipie.mashTimes[curDisplayedMashStep]);
       tempMashHours = convertToDisHours(inputRecipie.mashTimes[curDisplayedMashStep]);
       tempMashTemp = inputRecipie.mashTemps[curDisplayedMashStep];
       tempMashPumpOn = inputRecipie.mashPumpStates[curDisplayedMashStep];
       currentScreen = mashScreen;
       if(inputRecipie.numberOfMashSteps > curDisplayedMashStep + 1)tempMoreMashSteps = true;
       lcd.clear();
       break;
   }
   case mashInputScreen:
   {
      //store current Vars
      inputRecipie.mashTemps[curDisplayedMashStep] = tempMashTemp;
      inputRecipie.mashPumpStates[curDisplayedMashStep] = tempMashPumpOn;
      inputRecipie.mashTimes[curDisplayedMashStep] = convertToSeconds(tempMashHours,tempMashMins,tempMashSecs);
      curEdit = 0;
      lcd.clear();
      if(tempMoreMashSteps)
       {
          if(curDisplayedMashStep<inputRecipie.numberOfMashSteps)
          {
             //fill temp vars if they already exist
             curDisplayedMashStep++;
             tempMashTemp =  inputRecipie.mashTemps[curDisplayedMashStep];
             tempMashPumpOn = inputRecipie.mashPumpStates[curDisplayedMashStep];
             tempMashHours = convertToDisHours(inputRecipie.mashTimes[curDisplayedMashStep]);
             tempMashMins = convertToDisMins(inputRecipie.mashTimes[curDisplayedMashStep]);
             tempMashSecs = convertToDisSecs(inputRecipie.mashTimes[curDisplayedMashStep]);
             if(curDisplayedMashStep == inputRecipie.numberOfMashSteps - 1)
             {
              tempMoreMashSteps = false; 
             }else {
              tempMoreMashSteps = true;
             }
             return;
          }else
          {
             //newMashStep
             tempMoreMashSteps = false;
             curDisplayedMashStep++;
             tempMashTemp = 0;
             tempMashPumpOn = false;
             tempMashHours = 0;
             tempMashMins = 0;
             tempMashSecs = 0; 
          }
       }else
       {
          //go to wort screen
          //save total mash steps
          inputRecipie.numberOfMashSteps = curDisplayedMashStep+1;
          //handle case where they dont want a mash
          if (inputRecipie.numberOfMashSteps == 2 && inputRecipie.mashTimes[curDisplayedMashStep] == 0) {
            inputRecipie.numberOfMashSteps--;
          }
          tempMoreMashSteps = false;
          //go to Wort Screen....fill in times, temps, and hop times if they are there
          tempMashHours = convertToDisHours(inputRecipie.wortTotalSecs);
          tempMashMins = convertToDisMins(inputRecipie.wortTotalSecs);
          tempMashSecs = convertToDisSecs(inputRecipie.wortTotalSecs);
          tempWortTemp = inputRecipie.wortTemp;
          for(i=0; i<3; i++)
          {
            hopIntHours[i] = convertToDisHours(inputRecipie.hopAdditionIntervals[i]);
            hopIntMins[i] = convertToDisMins(inputRecipie.hopAdditionIntervals[i]);
            hopIntSecs[i] = convertToDisSecs(inputRecipie.hopAdditionIntervals[i]);
          }
    
          currentScreen = wortScreen;
          lcd.clear();
          curEdit = 0;
       }
       break;
    }
    
    case wortInputScreen:
    {
     //save wort
     inputRecipie.wortTemp = tempWortTemp;
     inputRecipie.wortTotalSecs = convertToSeconds(tempMashHours,tempMashMins,tempMashSecs);
     inputRecipie.numOfHopSteps=0;
     for(int i=0; i<3; i++)
     {
        if(convertToSeconds(hopIntHours[i],hopIntMins[i],hopIntSecs[i])>0)
        {
         inputRecipie.hopAdditionIntervals[i] = convertToSeconds(hopIntHours[i],hopIntMins[i],hopIntSecs[i]);
         inputRecipie.numOfHopSteps++;
        } 
     }
     curEdit = 0;
     currentScreen = saveQuestionScreen;
     lcd.clear();
     //
     break; 
    }
    
    case saveStartQuestionScreen:
    {
      if(tempSave)
      {
       //Go Save
       curEdit = 0;
       //clear Name var
       for(i =0; i<18; i++) tempName[i] = inputRecipie.name[i];
       currentScreen = saveNameScreen;
       lcd.clear();
      }else
      {
       //start brew!
       setCurrentRecipie(inputRecipie);
       readyToBrew = true;
      }
      break;
    }
  }
}


///////////////for going back to the last screen
void screenBack()
{
   switch(currentScreen.id) 
  {
   case saveNameInputScreen:
  {
    //go back to save question screen
    currentScreen = saveQuestionScreen;
    lcd.clear();
    curEdit = 0;
    break;
  } 
    
    
   case savedListPickingScreen:
   {
     curEdit = 0;
     lcd.clear();
     if(isCurSaving)
     {
      //go back to name screen
     currentScreen = saveNameScreen; 
     return;
     }
     if(curSavedDisplayPage == 0)
     {
      //go back to first question
      currentScreen = loadOrNewScreen;
      return;
     }
     curSavedDisplayPage--;

     break;
   }  
   
   case strikeInputScreen:
   {
     currentScreen = loadOrNewScreen;
     curEdit = 0;
     lcd.clear();
    break; 
   }
    
   case mashInputScreen:
   {
       if(curDisplayedMashStep>1)
       {
         //go back and fill temp vars
        curDisplayedMashStep--;
        tempMashTemp =  inputRecipie.mashTemps[curDisplayedMashStep];
        tempMashPumpOn = inputRecipie.mashPumpStates[curDisplayedMashStep];
        tempMashHours = convertToDisHours(inputRecipie.mashTimes[curDisplayedMashStep]);
        tempMashMins = convertToDisMins(inputRecipie.mashTimes[curDisplayedMashStep]);
        tempMashSecs = convertToDisSecs(inputRecipie.mashTimes[curDisplayedMashStep]);
        lcd.clear();
        tempMoreMashSteps = true;
       }else
       {
       //go To strike
       curDisplayedMashStep--;
       tempMashTemp = inputRecipie.mashTemps[curDisplayedMashStep];
       curEdit = 0;
       lcd.clear();
       currentScreen = strikeScreen;
       }
       break; 
   }
   
   case wortInputScreen:
   {
        //go back to Mash
        currentScreen = mashScreen;
        tempMashTemp =  inputRecipie.mashTemps[curDisplayedMashStep];
        tempMashPumpOn = inputRecipie.mashPumpStates[curDisplayedMashStep];
        tempMashHours = convertToDisHours(inputRecipie.mashTimes[curDisplayedMashStep]);
        tempMashMins = convertToDisMins(inputRecipie.mashTimes[curDisplayedMashStep]);
        tempMashSecs = convertToDisSecs(inputRecipie.mashTimes[curDisplayedMashStep]);
        lcd.clear();
        tempMoreMashSteps = false; 
        break;
   }
   
   case saveStartQuestionScreen:
   {
    curEdit = 0;
    currentScreen = wortScreen;
    lcd.clear();
    //go back to Wort screen
    break; 
   }
   
  } 
}
