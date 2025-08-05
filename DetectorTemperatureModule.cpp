#include <WebSocketsServer.h>
#include "DetectorTemperatureModule.h"

extern WebSocketsServer WSserver;
void WSsendTemperature(float val);

DetectorTemperatureModule::DetectorTemperatureModule():wire(pin),sensor(&wire){

}

void DetectorTemperatureModule::setSetup(){
  sensor.begin();
}

void DetectorTemperatureModule::DetectorState(){
  if(millis() - timerStart >= requestDelay) {
    sensor.requestTemperatures();
    temperature= sensor.getTempCByIndex(0);
    WSsendTemperature(temperature);
    timerStart=millis();
  }
}

float DetectorTemperatureModule::getTemperature(){
  return temperature;
}

void DetectorTemperatureModule::setRequestDelay(uint value){
  requestDelay=value;
}