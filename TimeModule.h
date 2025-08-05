#ifndef TIMEMODULE_H
#define TIMEMODULE_H
#include <Arduino.h>
#include <NTPClient.h>
#include <TimeLib.h>
#include <WiFiUdp.h>
#include "LampModule.h"

class TimeModule{
  public:
    enum class TimeUnits{
    SECONDS,
    MINUTES,
    HOURS,
    FULLTIME
  };
  TimeModule(LampModule* lampmod);
  void TimeEmulation();
  char* getTime(TimeUnits unit=TimeUnits::FULLTIME);
  String getDate();
  unsigned long getPureTime();
  void setSerial(int num);
  void setConnect(bool conn);
  void getTimeFromsv(bool conn);
  void setTime(unsigned long newtime);
  void setSetup(bool conn);
  bool isSleeping();
  void setSleeping(bool slp);
  void setSleepTimer(unsigned long newslptm);
  void sendTime();
  void Ping();
  private:
  uint32_t serial_num;
  unsigned long timer;
  unsigned long timeofgetfromsv;
  bool connect;
  WiFiUDP ntpUDP;
  NTPClient timeClient;
  LampModule* lampmod;
  unsigned long fulltime;
    char timereturn[9];
  bool sleeping;
  unsigned long sleeptimer;
  int day;
  int month;
  int year;
  bool dateupdated;
};
#endif