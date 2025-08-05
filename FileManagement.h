#ifndef FILEMANAGEMENT_H
#define FILEMANAGEMENT_H
#include <LittleFS.h>
#include "LampModule.h"
#include "TimeModule.h"
#include "WiFiModule.h"
#include "Utils.h" 

class FileManagement{
public:
  FileManagement();
  void setSetup();
  void setSettings(unsigned long newfulltime,String newwifi_name,String newwifi_password,uint32_t newserial_speed);
  String getSettings();
  void writeSettings();
  void readSettings();
  void applySettings(LampModule* lampmod,TimeModule* timemod,WiFiModule* wifimod);
  void setWifiName(String newname);
  void setWifiPassword(String newpass);
  void setSerialSpeed(uint32_t newspeed);
  void setTime(unsigned long newtime);
  void setLampStateRed(bool state);
  void setLampStateGreen(bool state);
  void setLampStateBlue(bool state);
  void setLampStateWhite(bool state);
  void setLampBrightRed(uint8_t br);
  void setLampBrightGreen(uint8_t br);
  void setLampBrightBlue(uint8_t br);
  void setLampBrightWhite(uint8_t br);
  void setLampTimerRed(String timer);
  void setLampTimerGreen(String timer);
  void setLampTimerBlue(String timer);
  void setLampTimerWhite(String timer);
  void setLampProfile(int profilename,unsigned long hour,unsigned long minute,unsigned long second);
  void setLampProfile(int profilename,unsigned long* profilemass,uint masssize);
  File getWebFiles(String name);
  void saveLoginData(String username,String password);
private:
  Utils utils;
  unsigned long fulltime;
  String wifi_name;   //50
  String wifi_password;  //100
  uint32_t serial_speed;  //104
  bool lamp_red_state;
  bool lamp_green_state;
  bool lamp_blue_state;
  bool lamp_white_state;
  uint8_t lamp_red_bright;
  uint8_t lamp_green_bright;
  uint8_t lamp_blue_bright;
  uint8_t lamp_white_bright;
  String lamp_red_timer;
  String lamp_green_timer;
  String lamp_blue_timer;
  String lamp_white_timer;
  unsigned long lamp_profile_1[5];
  unsigned long lamp_profile_2[5];
  unsigned long lamp_profile_3[5];
  unsigned long lamp_profile_4[5];
  String svusername;
  String svpassword;
  String getSettingByName(String settingsline,String name,char endchar);
};

#endif