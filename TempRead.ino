#include "OneWire.h"
#include "DallasTemperature.h"

//inport Pins
extern const int DS18S20_Pin = 5;//DS18S20 Signal pin on digital 7
extern const int HLTTemp = 70;

//Temperature chip i/o
OneWire ds(DS18S20_Pin);  // on digital pin 2


// arrays to hold device addresses
  DeviceAddress HLTTherm;

void setupTempSensor(void) {  
   ds.reset_search();
  if (!ds.search(HLTTherm)) Serial.println("Unable to find address for HLTTherm");

  getTempNew(HLTTemp);
}

float getTempNew(int vessel)
{
   float tempC;
   switch(vessel)
   {
    case HLTTemp:
        tempC = getTemp(HLTTherm);
        break;
   }
   return tempC;
}

//for testing
void printTemp(void) {
  float temperature = getTemp(HLTTherm);
  
  Serial.print(temperature);
  Serial.print("   Farenheight:");
  Serial.print(getTempF(temperature));
  
  Serial.println();
  
  delay(500); //just here to slow down the output so it is easier to read
}

//need to address
float getTemp(DeviceAddress addr){
  //returns the temperature from one DS18S20 in DEG Celsius

  byte data[12];

  /*if ( !ds.search(addr)) {
      //no more sensors on chain, reset search
      ds.reset_search();
      return -1000;
  }*/

  if ( OneWire::crc8( addr, 7) != addr[7]) {
      //Serial.println("CRC is not valid!");
      return -1000;
  }

  if ( addr[0] != 0x10 && addr[0] != 0x28) {
      //Serial.print("Device is not recognized");
      return -1000;
  }

  ds.reset();
  ds.select(addr);
  ds.write(0x44,1); // start conversion, with parasite power on at the end

  byte present = ds.reset();
  ds.select(addr);    
  ds.write(0xBE); // Read Scratchpad

  
  for (int i = 0; i < 9; i++) { // we need 9 bytes
    data[i] = ds.read();
  }
  
  ds.reset_search();
  
  byte MSB = data[1];
  byte LSB = data[0];

  float tempRead = ((MSB << 8) | LSB); //using two's compliment
  float TemperatureSum = tempRead / 16;
  
  
  return TemperatureSum;
  
}

float getTempF(float celcius)
{
   return ((celcius*9)/5)+32;
}
