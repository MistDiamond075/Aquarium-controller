#include "TimeModule.h"
#include <Arduino.h>
#include <NTPClient.h>
#include <TimeLib.h>
#include <WiFiUdp.h>
#include "LampModule.h"

TimeModule::TimeModule(LampModule* lmp):timeClient(ntpUDP, "time.windows.com"),timer(millis()),connect(false),serial_num(9600),lampmod(lmp),
timeofgetfromsv(0),sleeping(false),sleeptimer(0),dateupdated(false){ //seconds(0),hours(0),minutes(0),
}

void TimeModule::TimeEmulation(){
    fulltime+=(millis()-timer)/1000;
    timer=millis()-(millis()-timer) % 1000;
    // Serial.println(((fulltime / 3600) % 24) *3600 + ((fulltime / 60) % 60)*60 + fulltime % 60);
    //if(!lampmod->isAnyProfileActive()){
    //  lampmod->loadProfile(0);
      lampmod->isProfileTime(fulltime);
    //}else{
     //lampmod->setLampTime(((fulltime / 3600) % 24)  , ((fulltime / 60) % 60) , fulltime % 60); 
    //}
  if(millis()-timeofgetfromsv>=10800000){ //3 hours(10800*1000)
    getTimeFromsv(connect);
  }
  if(!sleeping){sleeptimer++;}
  if(sleeptimer>=2500){sleeping=true;}
  if(fulltime==86399){
   // Serial.println(fulltime);
   if(!dateupdated){
    day++;
    dateupdated=true;
    if(day>31){
      day=1;
      month++;
      if(month>12){
        month=1;
        year++;
      }
    }
   }
  }else{
    dateupdated=false;
  }
}

char* TimeModule::getTime(TimeUnits unit){
  char buffer[3];
  switch(unit){
    case TimeUnits::SECONDS:{
      return utoa(fulltime % 60, buffer, 10);
    }
    case TimeUnits::MINUTES:{
      return utoa((fulltime / 60) % 60, buffer, 10);
    }
    case TimeUnits::HOURS:{
      return utoa((fulltime / 3600) % 24, buffer, 10);
    }
    default:{
      snprintf(timereturn,sizeof(timereturn),"%02lu:%02lu:%02lu",(fulltime / 3600) % 24,(fulltime / 60) % 60,fulltime % 60);
      return timereturn;
    }
  }
}

String TimeModule::getDate(){
  String d;
  String m;
  if(day<10){
    d+="0";
  }
  d+=String(day);
  if(month<10){
    m+="0";
  }
  m+=String(month);
  return d+"."+m+"."+String(year);
}

void TimeModule::setSerial(int num){
  serial_num=num;
}

void TimeModule::setConnect(bool conn){
  connect=conn;
}

void TimeModule::getTimeFromsv(bool conn){
  if(!conn){
    return;
  }
  timeClient.update();
  fulltime=timeClient.getEpochTime();
  timeofgetfromsv=millis();
  //Serial.println(epochtime);
  struct tm *ptm=gmtime((time_t *)&fulltime);
  String svtime=timeClient.getFormattedTime();
  day=ptm->tm_mday;
  month=ptm->tm_mon+1;
  year=ptm->tm_year+1900;
  //dayofgetfromsv=ptm->tm_mday;
}

void TimeModule::setSetup(bool conn){
    if(!conn){
    return;
  }
  // GMT +1 = 3600
  // GMT -1 = -3600
  timeClient.begin();
  timeClient.setTimeOffset(18000);
  getTimeFromsv(conn);
  timer=millis();
}

unsigned long TimeModule::getPureTime(){
  return fulltime;
}

void TimeModule::setTime(unsigned long newtime){
  fulltime=newtime;
}

bool TimeModule::isSleeping(){
  return sleeping;
}

void TimeModule::setSleeping(bool slp){
  sleeping=slp;
}

void TimeModule::setSleepTimer(unsigned long newslptm){
  sleeptimer=newslptm;
}

void TimeModule::sendTime(){
  //if(Serial.available()>0){
    String snd=String(getPureTime());
    Serial.println("snd");
    Serial.print("");
    //if(Serial.readString()!="received"){
     // Ping();
    //}
  //}
}

void TimeModule::Ping(){
  if(Serial.available()>0){
    Serial.println("pingmega");
    if(Serial.readString()!="ok"){
      Serial.println("reboot");
      Serial.println("rebooting...");
    }
  }
}