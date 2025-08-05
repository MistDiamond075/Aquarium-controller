#ifndef LAMPMODULE_H
#define LAMPMODULE_H
#include <Arduino.h>
#include <climits>
#include <utility>
#include <WebSocketsServer.h>
#include "roo_collections_flat_small_hash_map.h"
#include "Utils.h"

class LampModule{
public:
  LampModule();
  ~LampModule();
  void setSetup();
  bool isRedOn();
  bool isGreenOn();
  bool isBlueOn();
  bool isWhiteOn();
  unsigned long getLampTimeUnformatted(uint8_t profile_num);
  void setLampTime(uint8_t profile_num,unsigned long hour,unsigned long minute,unsigned long second);
  void setLampTime(unsigned long hour,unsigned long minute,unsigned long second);
  void setProfile(int profilename, unsigned long redbright, unsigned long greenbright, unsigned long bluebright, unsigned long whitebright, unsigned long timerstart);
  void setProfile(int profilename, unsigned long* profilemass,uint masssize);
  void setProfilesEndTimer();
  void getProfileInfo(int profilename,unsigned long* resultmass,uint masssize, unsigned long& endTimer);
  void getProfileInfo(int profilename,unsigned long* resultmass,uint masssize);
  void getProfileActive(unsigned long* resultmass,uint masssize);
  int getProfileActive();
  void loadProfile(int profilename);
  void LampEmulation();
  void isProfileTime(unsigned long time);
  bool isAnyProfileActive();
  bool isProfileActive(int profilename);
  int8_t getProfileCount();
  void setSettingsLoaded(bool is_loaded);
private:
  struct Timer{
    unsigned long start;
    unsigned long end;

    Timer(unsigned long tmstart,unsigned long tmend):start(tmstart),end(tmend){}
    Timer():start(ULONG_MAX),end(ULONG_MAX){}
  };
  bool redon;
  bool greenon;
  bool blueon;
  bool whiteon;
  uint8_t redbright;
  uint8_t greenbright;
  uint8_t bluebright;
  uint8_t whitebright;
  Timer *timers;
  Utils utils;
  unsigned long endtimer;
  unsigned long profile1[5];
  unsigned long profile2[5];
  unsigned long profile3[5];
  unsigned long profile4[5];
  unsigned long defaultprofile[5];
  uint8_t profile_count;
  bool anyprofileactive;
  int activeprofile;
  bool settings_loaded;
  unsigned long setEndTimer(unsigned long newtimerstart);
};

#endif