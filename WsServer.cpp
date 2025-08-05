#include <Arduino.h>
#include <WebSocketsServer.h>
#include <ArduinoJson.h>
#include <map>
#include <utility>
#include "mbedtls/base64.h"
#include "WiFiModule.h"
#include "TimeModule.h"
#include "LampModule.h"
#include "DetectorTemperatureModule.h"
#include "FileManagement.h"
#include "Utils.h"

WebSocketsServer WSserver(60202);
FileManagement* WSfilemod;
TimeModule* tmod;
LampModule* lmod;
WiFiModule* wfmod;
DetectorTemperatureModule* dtmod;
Utils utils;

void WShandleWebSocket(uint8_t client_num, WStype_t type, uint8_t * payload, size_t length);
void WSgetAll(uint8_t clientId);
void WSgetTime(uint8_t clientId);
void WSgetWifi(JsonObject& req,uint8_t clientId);
void WSgetProfile(JsonObject& req, uint8_t clientId);
void WSgetDetectorValue(JsonObject& req, uint8_t clientId);
void WSgetTemperature(uint8_t clientId);
void WSsendActiveProfile(int profileId);
void WSsendTemperature(float val);
void WSupdateTime(JsonObject& data, uint8_t clientId);
void WSupdateWifi(JsonObject& data, uint8_t clientId);
void WSupdateProfile(JsonObject& data, uint8_t clientId);
bool WSsetProfileInfo(int8_t profile_num, unsigned long* arr, JsonObject& obj);

void WSsetSetup(FileManagement* fm, TimeModule* tm, LampModule* lm, WiFiModule* wfm,DetectorTemperatureModule* dtm) {
  //Serial.begin(9600);
  WSfilemod = fm;
  lmod = lm;
  tmod = tm;
  wfmod = wfm;
  dtmod=dtm;
  if (wfmod->isConnected()) {
    WSserver.begin();
    WSserver.onEvent(WShandleWebSocket);
    Serial.println("websocket WSserver started");
  } else {
    Serial.println("websocket WSserver not started: no internet connection");
  }
}

void WShandleWebSocket(uint8_t client_num, WStype_t type, uint8_t * payload, size_t length){
    if (type == WStype_TEXT) {
    String msg = String((char*)payload);
    JsonDocument doc;
    DeserializationError err = deserializeJson(doc, msg);
    if (err) {
      WSserver.sendTXT(client_num, "{\"error\":\"invalid JSON\"}");
      return;
    }
    //Serial.println(doc);
    String event = doc["event"];
    String type = doc["type"];
    if(event == "get"){
      if (type == "all") {
        WSgetAll(client_num);
      }
      else if(type=="time"){
        WSgetTime(client_num);
      }
      else if(type=="detector"){
        JsonObject data=doc["data"];
        WSgetDetectorValue(data,client_num);
      }
      else if(type=="wifi"){
        JsonObject data=doc["data"];
        WSgetWifi(data,client_num);
      }
      else if(type=="profile"){
        JsonObject data=doc["data"];
        WSgetProfile(data,client_num);
      }
    }else if(event == "update"){
      JsonObject data=doc["data"];
      if (type == "time") {
        WSupdateTime(data,client_num);
      }
      else if (type == "wifi") {
        WSupdateWifi(data,client_num);
      }
      else if(type == "profile"){
        WSupdateProfile(data,client_num);
      }
    }else if(type=="connected"){
      WSsendActiveProfile(client_num);
    }
    else {
      WSserver.sendTXT(client_num, "{\"error\":\"bad request\"}");
    }
  }
}

void WSgetAll(uint8_t clientId) {
  uint8_t iterator = 0;
  unsigned long profinfo[5];
  JsonDocument resp;
  resp["event"]="response";
  resp["type"]="all";
  JsonObject data=resp["data"].to<JsonObject>();
  JsonArray profiles = data["profiles"].to<JsonArray>();
  for (int i = 1; i < 5; i++) {
     unsigned long timerEnd;
    lmod->getProfileInfo(i, profinfo, 5,timerEnd);
    JsonObject profile = profiles.add<JsonObject>();
    if(lmod->isProfileActive(i)){
      profile["active"]=true;
    }
    profile["name"] = "profile " + String(i);
    profile["red"] = profinfo[0];
    profile["green"] = profinfo[1];
    profile["blue"] = profinfo[2];
    profile["white"] = profinfo[3];
    profile["start"] = profinfo[4];
    profile["end"]=timerEnd;
  }
  data["password"]=wfmod->getPassword();
  data["ssid"]=wfmod->getSSID();
  data["quality"]=wfmod->getSignalQuality();
  data["time"]= tmod->getTime();
  String output;
  serializeJson(resp, output);
  WSserver.sendTXT(clientId, output);
}

void WSgetTime(uint8_t clientId){
  JsonDocument resp;
  resp["event"]="response";
  resp["type"]="time";
  JsonObject data=resp["data"].to<JsonObject>();
  data["time"]=tmod->getTime();
  data["seconds"]=tmod->getTime(tmod->TimeUnits::SECONDS);
  data["minutes"]=tmod->getTime(tmod->TimeUnits::MINUTES);
  data["hours"]=tmod->getTime(tmod->TimeUnits::HOURS);
  String output;
  serializeJson(resp, output);
  WSserver.sendTXT(clientId, output);
}

void WSgetWifi(JsonObject& req,uint8_t clientId){
  JsonDocument resp;
  resp["event"]="response";
  resp["type"]="wifi";
  JsonObject data=resp["data"].to<JsonObject>();
  if(req["fulldata"].is<bool>()){
    if(req["fulldata"]){
      data["ssid"]=wfmod->getSSID();
      data["password"]=wfmod->getPassword();
    }
    data["quality"]=wfmod->getSignalQuality();
    String output;
    serializeJson(resp, output);
    WSserver.sendTXT(clientId, output);
  }else{
    WSserver.sendTXT(clientId, "{\"error\":\"missing parameters\"}");
  }
}

void WSgetProfile(JsonObject& req, uint8_t clientId){
  bool singleProfile=req["id"].is<int>() && !req["id"].isNull();
  bool activeProfile=req["active"];
  JsonDocument resp;
  resp["event"]="response";
  resp["type"]=singleProfile ? "profile" : "profiles";
  JsonObject data=resp["data"].to<JsonObject>();
  if(activeProfile){
    int8_t profile_num=lmod->getProfileActive();
    unsigned long profinfo[5];
    JsonObject profile = data["profile"].to<JsonObject>();
    WSsetProfileInfo(profile_num,profinfo,profile); 
  }
  else if(singleProfile){
    int8_t profile_num=req["id"];
    unsigned long profinfo[5];
    JsonObject profile = data["profile"].to<JsonObject>();
    WSsetProfileInfo(profile_num,profinfo,profile);
  }else{
    JsonArray profiles = data["profiles"].to<JsonArray>();
    for(int i = 1; i < 5; i++){
      unsigned long profinfo[5];
      JsonObject profile = profiles.add<JsonObject>();
      WSsetProfileInfo(i,profinfo,profile);
    }
  }
  String output;
  serializeJson(resp, output);
  WSserver.sendTXT(clientId, output);
}

void WSgetDetectorValue(JsonObject& req,uint8_t clientId){
  String id=req["id"];
  if(id=="temperature"){
    WSgetTemperature(clientId);
  }
}

void WSgetTemperature(uint8_t clientId){
  JsonDocument resp;
  resp["event"]="response";
  resp["type"]="detector";
  JsonObject data=resp["data"].to<JsonObject>();
  data["id"]="temperature";
  data["value"]=dtmod->getTemperature();
  String output;
  serializeJson(resp, output);
  WSserver.sendTXT(clientId, output);
}

void WSupdateTime(JsonObject& data,uint8_t clientId) {
  if (!data["hours"].is<int>() || !data["minutes"].is<int>() || !data["seconds"].is<int>()) {
    WSserver.sendTXT(clientId, "{\"error\":\"missing parameters\"}");
    return;
  }
  uint8_t hours = data["hours"];
  uint8_t minutes = data["minutes"];
  uint8_t seconds = data["seconds"];
  if (hours > 23 || minutes > 59 || seconds > 59) {
    WSserver.sendTXT(clientId, "{\"error\":\"invalid time\"}");
    return;
  }
  unsigned long ultime = hours * 3600 + minutes * 60 + seconds;
  Serial.printf("Time set: %02u:%02u:%02u (%lu sec)\n", hours, minutes, seconds, ultime);
  tmod->setTime(ultime);
  WSfilemod->setTime(ultime);
  WSfilemod->writeSettings();
  WSserver.sendTXT(clientId, "{\"updated\":true}");
}

void WSupdateWifi(JsonObject& data,uint8_t clientId) {
  if (!data["ssid"].is<String>() && !data["password"].is<String>()) {
    WSserver.sendTXT(clientId, "{\"error\":\"missing parameters\"}");
    return;
  }
  String ssid = data["ssid"];
  String pass = data["password"];
  if (ssid.length()>40) {
    WSserver.sendTXT(clientId, "{\"error\":\"ssid too long\"}");
    return;
  }
  if (pass.length()>40) {
    WSserver.sendTXT(clientId, "{\"error\":\"password too long\"}");
    return;
  }
  if (ssid != "") {
    Serial.println("ssid: " + ssid);
    wfmod->setSSID(ssid);
    WSfilemod->setWifiName(ssid);
  }
  if (pass != "") {
    Serial.println("pass: " + pass);
    wfmod->setPassword(pass);
    WSfilemod->setWifiPassword(pass);
  }
  WSfilemod->writeSettings();
  WSserver.sendTXT(clientId, "{\"updated\":true}");
}

void WSupdateProfile(JsonObject& data,uint8_t clientId) {
  if (!data["id"].is<int>() || !data["red"].is<int>() || !data["green"].is<int>() || !data["blue"].is<int>() || !data["white"].is<int>() || !data["timer"].is<String>()) {
    WSserver.sendTXT(clientId, "{\"error\":\"missing parameters\"}");
    return;
  }
  unsigned long prof[5];
  uint profnum = data["id"];
  uint colorRed = data["red"];
  uint colorGreen = data["green"];
  uint colorBlue = data["blue"];
  uint colorWhite = data["white"];
  String newtimer = data["timer"];
  if(profnum<1 || profnum>4){
    WSserver.sendTXT(clientId, "{\"error\":\"profile isn't exist\"}");
    return;
  }
  Serial.println(profnum);
  unsigned long fulltime=ULONG_MAX;
  if (newtimer != "" && newtimer.indexOf(':') != -1) {
    String h = newtimer.substring(0, newtimer.indexOf(':'));
    newtimer=newtimer.substring(newtimer.indexOf(':')+1);
    if (newtimer.indexOf(':') != newtimer.length() - 1) {
      String m = newtimer.substring(0, newtimer.indexOf(':'));
      if (utils.isStringNumber(h) && utils.isStringNumber(m)) {
        fulltime = h.toInt() * 3600 + m.toInt() * 60;
      }
    }
  }
  unsigned long newProfile[5];
  newProfile[0]=colorRed;
  newProfile[1]=colorGreen;
  newProfile[2]=colorBlue;
  newProfile[3]=colorWhite;
  newProfile[4]=fulltime;
  lmod->setProfile(profnum, newProfile, 5);
  WSfilemod->setLampProfile(profnum, newProfile, 5);
  WSfilemod->writeSettings();
  WSserver.sendTXT(clientId, "{\"updated\":true}");
}

void WSsendActiveProfile(int profileId){
  JsonDocument resp;
  Serial.println("profileID: "+String(profileId));
  resp["event"]="response";
  resp["type"]="profile";
  JsonObject data=resp["data"].to<JsonObject>();
  unsigned long arr[5];
  if(!WSsetProfileInfo(profileId,arr,data)){
    return;
  }
  String output;
  serializeJson(resp, output);
  WSserver.broadcastTXT(output);
}

void WSsendTemperature(float val){
   JsonDocument resp;
  resp["event"]="response";
  resp["type"]="detector";
  JsonObject data=resp["data"].to<JsonObject>();
  data["id"]="temperature";
  data["value"]=val;
  String output;
  serializeJson(resp, output);
  WSserver.broadcastTXT(output);
}

bool WSsetProfileInfo(int8_t profile_num, unsigned long* arr, JsonObject& obj){
    unsigned long timerEnd;
    if(profile_num<1 || profile_num>4){
      return false;
    }
    lmod->getProfileInfo(profile_num, arr, 5,timerEnd);
    if(lmod->isProfileActive(profile_num)){
      obj["active"]=true;
    }
    obj["name"] = "profile " + String(profile_num);
    obj["red"] = arr[0];
    obj["green"] = arr[1];
    obj["blue"] = arr[2];
    obj["white"] = arr[3];
    obj["start"] = arr[4];
    obj["end"]=timerEnd;
    return true;
};