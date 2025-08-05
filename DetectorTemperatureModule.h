#ifndef DETECTORTEMPERATUREMODULE_H
#define DETECTORTEMPERATUREMODULE_H
#include <OneWire.h>
#include <DallasTemperature.h>

class DetectorTemperatureModule{
  public:
    DetectorTemperatureModule();
    void setSetup();
    void DetectorState();
    float getTemperature();
    void setRequestDelay(uint value);
  private:
    uint pin=32;
    float temperature=0;
    OneWire wire;
    DallasTemperature sensor;
    unsigned long timerStart = millis();
    uint requestDelay=4000;
};

#endif