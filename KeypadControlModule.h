#ifndef KEYPADCONTROLMODULE_H
#define KEYPADCONTROLMODULE_H
#include <Wire.h>
#include <Adafruit_MPR121.h>
#include "TimeModule.h"

class KeypadControlModule{
public:
  KeypadControlModule();
  void setSetup();
  int8_t KeypadState();
  void setSwitching(bool isSwitched);
  bool getSwitch();
  void setTimemodule(TimeModule* tmmod);
private:
  Adafruit_MPR121 ads; 
  bool switching;
  TimeModule* timemod;
};

#endif
