#include "LampModule.h"
#include <Arduino.h>
#include "roo_collections_flat_small_hash_map.h"
#include <climits>
#include <algorithm>
#include <utility>

extern WebSocketsServer WSserver;
void WSsendActiveProfile(int profileId);

LampModule::~LampModule() {
  if (timers) {
    delete[] timers;
    timers = nullptr;
  }
}

LampModule::LampModule()
  : redon(false), greenon(false), blueon(false), whiteon(false), redbright(0), greenbright(0), bluebright(0), whitebright(0),
    anyprofileactive(false), endtimer(ULONG_MAX), activeprofile(0), profile_count(4), settings_loaded(false) {
  timers = new Timer[profile_count];
}

void LampModule::setSetup() {
  Serial.begin(9600);
  if (!settings_loaded) {
    for (int i = 0; i < 4; i++) {
      profile1[i] = 0;
      profile2[i] = 0;
      profile3[i] = 0;
      profile4[i] = 0;
    }
    profile1[4] = ULONG_MAX;
    profile2[4] = ULONG_MAX;
    profile3[4] = ULONG_MAX;
    profile4[4] = ULONG_MAX;
  }
  for (int i = 0; i < 4; i++) {
    defaultprofile[i] = ULONG_MAX;
  }
  defaultprofile[4] = ULONG_MAX;
}

int8_t LampModule::getProfileCount() {
  return profile_count;
}

bool LampModule::isRedOn() {
  return redon;
}

bool LampModule::isGreenOn() {
  return greenon;
}

bool LampModule::isBlueOn() {
  return blueon;
}

bool LampModule::isWhiteOn() {
  return whiteon;
}

unsigned long LampModule::getLampTimeUnformatted(uint8_t profile_num) {
  if (profile_num > profile_count || profile_num < 0) {
    return defaultprofile[4];
  }
  switch (profile_num) {
    case 1:
      {
        return profile1[4];
        break;
      }
    case 2:
      {
        return profile2[4];
        break;
      }
    case 3:
      {
        return profile3[4];
        break;
      }
    case 4:
      {
        return profile4[4];
        break;
      }
    default:
      {
        return ULONG_MAX;
      }
  }
}

void LampModule::setLampTime(uint8_t profile_num, unsigned long hour, unsigned long minute, unsigned long second) {
  LampEmulation();
  if ((hour * 3600 + minute * 60 + second) >= getLampTimeUnformatted(profile_num)) {
    Serial.println("++++++++++++++++++++++++++++++++++++++++++++++++++++++");
    anyprofileactive = false;
    loadProfile(0);
    activeprofile = 0;
    LampEmulation();
  }
}

void LampModule::setLampTime(unsigned long hour, unsigned long minute, unsigned long second) {
  LampEmulation();
  if ((hour * 3600 + minute * 60 + second) >= endtimer) {
    Serial.println("++++++++++++++++++++++++++++++++++++++++++++++++++++++");
    anyprofileactive = false;
    loadProfile(0);
    activeprofile = 0;
    LampEmulation();
  }
}

void LampModule::setProfile(int profilename, unsigned long redbright, unsigned long greenbright, unsigned long bluebright, unsigned long whitebright, unsigned long timerstart) {
  Serial.println(")))))))))))))) " + String(profilename));

  auto ProfileBuilder = [redbright, greenbright, bluebright, whitebright, timerstart](unsigned long* profile) {
    profile[0] = redbright;
    profile[1] = greenbright;
    profile[2] = bluebright;
    profile[3] = whitebright;
    profile[4] = timerstart;
  };
  switch (profilename) {
    case 1:
      {
        ProfileBuilder(profile1);
        break;
      }
    case 2:
      {
        ProfileBuilder(profile2);
        break;
      }
    case 3:
      {
        ProfileBuilder(profile3);
        break;
      }
    case 4:
      {
        ProfileBuilder(profile4);
        break;
      }
  }
  timers[profilename - 1].start = timerstart;
  timers[profilename - 1].end = setEndTimer(timerstart);
}

void LampModule::setProfile(int profilename, unsigned long* profilemass, uint masssize) {
  Serial.println("profilename: " + String(profilename));
  switch (profilename) {
    case 1:
      {
        utils.copyArray(masssize, 5, profile1, profilemass);
        break;
      }
    case 2:
      {
        utils.copyArray(masssize, 5, profile2, profilemass);
        break;
      }
    case 3:
      {
        utils.copyArray(masssize, 5, profile3, profilemass);
        break;
      }
    case 4:
      {
        utils.copyArray(masssize, 5, profile4, profilemass);
        break;
      }
  }
  unsigned long timerstart = profilemass[4];
  timers[profilename - 1].start = timerstart;
  timers[profilename - 1].end = setEndTimer(timerstart);
}

void LampModule::setProfilesEndTimer() {
  unsigned long profiles_timers[profile_count];
  bool nextDay=false;
  for (int i = 0; i < profile_count; i++) {
    profiles_timers[i] = timers[i].start;
  }
  utils.insertionSort(profiles_timers, profile_count);
  for (int i = 0; i < profile_count; i++) {
//    Serial.println(String(profiles_timers[i])+'\t'+String(profiles_timers[i+1]));
    timers[i].start = profiles_timers[i];
    if(i!=profile_count-1){
      timers[i].end=profiles_timers[i+1];
    }
    Serial.println(String(timers[i].start)+'\t'+String(timers[i].end));
  }
  timers[profile_count-1].end=profiles_timers[0]+86000;
}

void LampModule::getProfileInfo(int profilename, unsigned long* resultmass, uint masssize) {
  switch (profilename) {
    case 1:
      {
        utils.copyArray(masssize, 5, resultmass, profile1);
        break;
      }
    case 2:
      {
        utils.copyArray(masssize, 5, resultmass, profile2);
        break;
      }
    case 3:
      {
        utils.copyArray(masssize, 5, resultmass, profile3);
        break;
      }
    case 4:
      {
        utils.copyArray(masssize, 5, resultmass, profile4);
        break;
      }
    default:
      {
        utils.copyArray(masssize, 5, resultmass, defaultprofile);
      }
  }
}

void LampModule::getProfileInfo(int profilename, unsigned long* resultmass, uint masssize, unsigned long& endTimer) {
  getProfileInfo(profilename, resultmass, masssize);
  for(int i=0;i<profile_count;i++){
      Serial.println(String(timers[i].start)+'\t'+String(resultmass[4])+'\t'+String(timers[i].end));
      if(timers[i].start==resultmass[4]){
        endTimer = timers[i].end>=86400 ? timers[i].end-86400 : timers[i].end;
        break;
      }
    }
}

void LampModule::getProfileActive(unsigned long* resultmass, uint masssize) {
  for (int i = 1; i < profile_count; i++) {
    if (isProfileActive(i)) {
      getProfileInfo(i, resultmass, masssize);
    }
  }
}

int LampModule::getProfileActive() {
  for (int i = 1; i < profile_count; i++) {
    if (isProfileActive(i)) {
      return i;
    }
  }
  return 0;
}

void LampModule::loadProfile(int profilename) {
  unsigned long prof[5];
  getProfileInfo(profilename, prof, sizeof(prof) / sizeof(prof[0]));
  if (memcmp(prof, defaultprofile, sizeof(prof)) != 0) {
    redon = prof[0] > 0;
    greenon = prof[1] > 0;
    blueon = prof[2] > 0;
    whiteon = prof[3] > 0;
    redbright = prof[0];
    greenbright = prof[1];
    bluebright = prof[2];
    whitebright = prof[3];
    for(int i=0;i<profile_count;i++){
      Serial.println("start: "+String(timers[i].start)+"\t prof[4]: "+String(prof[4])+"\t end: "+String(timers[i].end));
      if(timers[i].start==prof[4]){
        endtimer = timers[i].end>=86400 ? timers[i].end-86400 : timers[i].end;
        break;
      }
    }
    Serial.println(endtimer);
    //Serial.println("profileID from lampmod: " + String(profilename));
    WSsendActiveProfile(profilename);
  }
}

void LampModule::LampEmulation() {
  /*pcf.write(0,redbright*2.55);
    pcf.write(1,greenbright*2.55);
   pcf.write(2,bluebright*2.55);
   // Serial.println("bluebright "+String(bluebright*2.55));
   pcf.write(3,whitebright*2.55);*/
}

void LampModule::isProfileTime(unsigned long fulltime) {
  unsigned long time = ((fulltime / 3600) % 24) * 3600 + ((fulltime / 60) % 60) * 60 + fulltime % 60;

  if(endtimer!=ULONG_MAX){
    if(time<endtimer){
      return;
    }
  }
  for (int i = 1; i <= profile_count; i++) {
    unsigned long prof[5];
    getProfileInfo(i, prof, sizeof(prof) / sizeof(prof[0]));
    if (prof[4]) {
      unsigned long tmr = prof[4];
      unsigned long start=ULONG_MAX;
      unsigned long end=ULONG_MAX;
      for(int i=0;i<profile_count;i++){
        if(tmr==timers[i].start){
          start=timers[i].start;
          end=timers[i].end;
        }
      }
      //Serial.println("now: "+String(((end>=86000 && time<start) ? 86000+time : time))+"\t start: "+String(start)+"\t end: "+String(end));
      if(start==ULONG_MAX || end==ULONG_MAX){
        continue;
      }
      //Serial.println(String(activeprofile)+"\t"+String(i));
      if (((end>=86000 && time<start) ? 86000+time : time) >= start && ((end>=86000 && time<start) ? 86000+time : time)<end) {
        anyprofileactive = true;
        if(activeprofile!=i){
          loadProfile(i);
          activeprofile = i;
          Serial.println("==================activating profile==================");
          Serial.println(String(activeprofile)+"\t"+String(i));
        }
        LampEmulation();
        break;
      }
    }
  }
}

bool LampModule::isAnyProfileActive() {
  return anyprofileactive;
}

bool LampModule::isProfileActive(int profilename) {
  if (activeprofile == profilename) {
    //activeprofile=profilename;
    //loadProfile(profilename);
    //LampEmulation();
    return true;
  }
  return false;
}

unsigned long LampModule::setEndTimer(unsigned long newtimerstart) {
  unsigned long end=ULONG_MAX;
  int index=-1;
  for(int i=0;i<profile_count;i++){
    if(newtimerstart>timers[i].start){
      end=timers[i].end;
      index=i;
    }
  }
  if(index>=0 && index!=profile_count-1){
    timers[index].end=newtimerstart;
  }
  return index==profile_count-1 ? end+86000 : end;
}

void LampModule::setSettingsLoaded(bool is_loaded) {
  settings_loaded = is_loaded;
}