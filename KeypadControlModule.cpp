#include "KeypadControlModule.h"
#include <Wire.h>
#include <Adafruit_MPR121.h>

#define MPR121_I2C_ADDRESS 0x5A

KeypadControlModule::KeypadControlModule():switching(false){

}

void KeypadControlModule::setSetup(){
  //ads=Adafruit_MPR121();
  Serial.begin(9600);
  Serial.println("MPR121 test");
  Wire.begin(21, 22);
  Wire.setClock(100000);
  delay(100);
  while (!ads.begin(MPR121_I2C_ADDRESS,&Wire)) {
    Serial.println("MPR121 not found, check wiring?");
    // (1);
  }
  Serial.println("MPR121 found!");
}

int8_t KeypadControlModule::KeypadState(){
  static uint32_t lastRead = 0;
  uint32_t now = millis();

  if (now - lastRead < 50) return -1; 
  lastRead = now;

  uint16_t touched = ads.touched();
  //Serial.print(":: ");Serial.println(ads.touched());
  if(touched!=0){
  if(!switching){
    timemod->setSleeping(false);
    timemod->setSleepTimer(0);
  for (uint8_t i = 0; i < 12; i++) {
    if (touched & (1 << i)) {
      //Serial.println(i);
      return i; 
    }
  }
  }  
  }else{
    switching=false;
  }
  return -1;
}

void KeypadControlModule::setSwitching(bool isSwitched){
  switching=isSwitched;
}

bool KeypadControlModule::getSwitch(){
  return switching;
}

void KeypadControlModule::setTimemodule(TimeModule* tmmod){
  timemod=tmmod;
}