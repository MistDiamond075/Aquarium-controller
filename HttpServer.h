#ifndef HTTPSERVER_H
#define HTTPSERVER_H
#define JWT_KEY "093xta5782g0144snfq"
#include <Arduino.h>
#include <WebServer.h>
#include <ArduinoJson.h>
#include <map>
#include <utility>
#include "mbedtls/base64.h"
#include "WiFiModule.h"
#include "TimeModule.h"
#include "LampModule.h"
#include "FileManagement.h"
#include "Utils.h"

class HttpServer{
public:  
  HttpServer();
  void setSetup(FileManagement* fm, TimeModule* tm, LampModule* lm, WiFiModule* wfm);
  void handleRoot();
  void handleLoginpage();
  void getNonExistentPage();
  void getServerInfo();
  void getTime();
  void updateTime();
  void updateWifi();
  void updateProfile();
  void updateUser();
  void handleAuth();
  void handleUnauth();
  void RemoteControl();
  void getCss(String path);
  void getJs(String path);
  WebServer& getServer();
  enum class DateTimePattern{
    HHMMSS,
    HHMM,
    HHMMSS_DDMMYYYY,
    HHMMSS_DDMMYY,
    HHMM_DDMMYYYY,
    HHMM_DDMMYY,
    DDMMYYYY,
    DDMMYY
  };
private:
  WebServer server;
  Utils utils;
  WiFiModule* wifimod;
  TimeModule* timemod;
  LampModule* lampmod;
  FileManagement* fmgmt;
  uint16_t sv_port=80;
  String svusername;
  String svpassword;
  String createJWT(String username);
  bool checkJWT(String jwt);
  String base64Encode(const String& input);
  String base64Decode(const String& input);
  bool checkAuth(WebServer& server);
  bool matchTimePattern(const String& time,DateTimePattern pattern=DateTimePattern::HHMM);
};

#endif