#include <LittleFs.h>
#include "LampModule.h"
#include "TimeModule.h"
#include "WiFiModule.h"
#include "FileManagement.h"
#include <climits>

FileManagement::FileManagement()
  : serial_speed(9600),fulltime(0),wifi_name(""),wifi_password(""),lamp_red_state(0),lamp_green_state(0),lamp_blue_state(0),lamp_white_state(0),lamp_red_bright(200),
    lamp_green_bright(200),lamp_blue_bright(200),lamp_white_bright(200), lamp_white_timer(""), lamp_green_timer(""), lamp_blue_timer(""), lamp_red_timer(""),
   svusername("admin"),svpassword("admin") {}

void FileManagement::setSetup() {
  Serial.begin(serial_speed);
  if (!LittleFS.begin()) {
    Serial.println("Can't write settings");
    LittleFS.format();
    if (!LittleFS.begin()) {
      Serial.println("fs formatted, but still not started");
    } else {
      Serial.println("formatted successfully");
    }
    return;
  } else {
    Serial.println("LittleFS started");
  }
  if(!LittleFS.exists("/config")){
    LittleFS.mkdir("/config");
  }
  if(!LittleFS.exists("/sv")){
    LittleFS.mkdir("/sv");
  }
  Serial.print("file exist:");
  Serial.println(LittleFS.exists("/config/settings.txt"));
  Serial.print("lamp mass size:");
  Serial.print(sizeof(lamp_profile_1) / sizeof(lamp_profile_1[0]));
  for (int i = 0; i < (sizeof(lamp_profile_1) / sizeof(lamp_profile_1[0])); i++) {
    lamp_profile_1[i] = ULONG_MAX;
    lamp_profile_2[i] = ULONG_MAX;
    lamp_profile_3[i] = ULONG_MAX;
    lamp_profile_4[i] = ULONG_MAX;
  }
}

void FileManagement::setSettings(unsigned long newfulltime, String newwifi_name, String newwifi_password, uint32_t newserial_speed) {
  fulltime = newfulltime;
  wifi_name = newwifi_name;
  wifi_password = newwifi_password;
  serial_speed = newserial_speed;
}
//C:\Users\egoru\AppData\Local\Temp\arduino
String FileManagement::getSettings() {
  readSettings();
  String ret = "fulltime: " + String(fulltime) + " Wifi name: " + String(wifi_name) + " Wifi password: " + String(wifi_password) + " Upload speed: " + String(serial_speed)
               + " lamp_red_state: " + String(lamp_red_state) + " lamp_green_state: " + String(lamp_green_state) + " lamp_blue_state: " + String(lamp_blue_state) + " lamp_white_state: "
               + String(lamp_white_state) + " lamp_red_bright: " + String(lamp_red_bright) + " lamp_green_bright: " + String(lamp_green_bright) + " lamp_blue_bright: " + String(lamp_blue_bright) + " lamp_white_bright: " + String(lamp_white_bright) + " lamp_red_timer: " + String(lamp_red_timer) + " lamp_green_timer: " + String(lamp_green_timer) + " lamp_blue_timer: " + String(lamp_blue_timer) + " lamp_white_timer: " + String(lamp_white_timer) + " profile_1: " + utils.convertULArrayToString(lamp_profile_1, sizeof(lamp_profile_1) / sizeof(lamp_profile_1));
  return ret;
}

void FileManagement::writeSettings() {
  File file = LittleFS.open("/config/settings.txt", "w");
  if (!file) {
    Serial.println("Failed to open file");
    file = LittleFS.open("/config/settings.txt", "w",true);
    if(!file){
      Serial.println("failed to create file");
      return;
    }
  }
  // if(wifi_name)
  file.print("fulltime:");
  file.print(fulltime);
  file.print(" wifiname:");
  file.print(wifi_name);
  file.print(" wifipass:");
  file.print(wifi_password);
  file.print(" uploadspd:");
  file.print(serial_speed);
  file.print(" lamp_red_state:");
  file.print(lamp_red_state);
  file.print(" lamp_green_state:");
  file.print(lamp_green_state);
  file.print(" lamp_blue_state:");
  file.print(lamp_blue_state);
  file.print(" lamp_white_state:");
  file.print(lamp_white_state);
  file.print(" lamp_red_bright:");
  file.print(lamp_red_bright);
  file.print(" lamp_green_bright:");
  file.print(lamp_green_bright);
  file.print(" lamp_blue_bright:");
  file.print(lamp_blue_bright);
  file.print(" lamp_white_bright:");
  file.print(lamp_white_bright);
  file.print(" lamp_red_timer:");
  file.print(lamp_red_timer);
  file.print(" lamp_green_timer:");
  file.print(lamp_green_timer);
  file.print(" lamp_blue_timer:");
  file.print(lamp_blue_timer);
  file.print(" lamp_white_timer:");
  file.print(lamp_white_timer);
  file.print(" lamp_profile_1:");
  file.print(utils.convertULArrayToString(lamp_profile_1, sizeof(lamp_profile_1) / sizeof(lamp_profile_1[0])));
  file.print(" lamp_profile_2:");
  file.print(utils.convertULArrayToString(lamp_profile_2, sizeof(lamp_profile_2) / sizeof(lamp_profile_2[0])));
  file.print(" lamp_profile_3:");
  file.print(utils.convertULArrayToString(lamp_profile_3, sizeof(lamp_profile_3) / sizeof(lamp_profile_3[0])));
  file.print(" lamp_profile_4:");
  file.print(utils.convertULArrayToString(lamp_profile_4, sizeof(lamp_profile_4) / sizeof(lamp_profile_4[0])));
  file.close();
  Serial.println("done");
}

void FileManagement::readSettings() {
  String settingsline;
  if (LittleFS.exists("/config/settings.txt")) {
    File file = LittleFS.open("/config/settings.txt", "r");
    if (!file) {
      Serial.println("Failed to open file");
      return;
    }
    Serial.println("file opened");
    while (file.available()) {
      settingsline += char(file.read());
    }
    file.close();
    //Serial.println("++"+settingsline);
    //  Serial.println(":::::::fulltime");
    fulltime = getSettingByName(settingsline,"fulltime:",' ').toInt();
    wifi_name = getSettingByName(settingsline,"wifiname:",' ');
    wifi_password = getSettingByName(settingsline,"wifipass:",' ');
    serial_speed = getSettingByName(settingsline,"uploadspd:",' ').toInt();
    // Serial.println("*********"+settingsline.substring(indexstart,indexend)+"********");
    //Serial.print("========= "+String(utils.convertStringToBoolean(settingsline.substring(indexstart,indexend))));
    ///  Serial.println("--------------");
    lamp_red_state = utils.convertStringToBoolean(getSettingByName(settingsline,"lamp_red_state:",' '));
    //Serial.println(lamp_red_state);
    lamp_green_state = utils.convertStringToBoolean(getSettingByName(settingsline,"lamp_green_state:",' '));
    lamp_blue_state = utils.convertStringToBoolean(getSettingByName(settingsline,"lamp_blue_state:",' '));
    //settingsline.indexOf(' ', indexstart);
    lamp_white_state = utils.convertStringToBoolean(getSettingByName(settingsline,"lamp_white_state:",' '));
    lamp_red_bright = getSettingByName(settingsline,"lamp_red_bright:",' ').toInt();
    lamp_green_bright = getSettingByName(settingsline,"lamp_green_bright:",' ').toInt();
    lamp_blue_bright = getSettingByName(settingsline,"lamp_blue_bright:",' ').toInt();
    lamp_white_bright = getSettingByName(settingsline,"lamp_white_bright:",' ').toInt();
    lamp_red_timer = getSettingByName(settingsline,"lamp_red_timer:",' ');
    lamp_green_timer = getSettingByName(settingsline,"lamp_green_timer:",' ');
    lamp_blue_timer = getSettingByName(settingsline,"lamp_blue_timer:",' ');
    lamp_white_timer = getSettingByName(settingsline,"lamp_white_timer:",' ');
    utils.convertStringToULArray(getSettingByName(settingsline,"lamp_profile_1:",' '), lamp_profile_1);
    utils.convertStringToULArray(getSettingByName(settingsline,"lamp_profile_2:",' '), lamp_profile_2);
    utils.convertStringToULArray(getSettingByName(settingsline,"lamp_profile_3:",' '), lamp_profile_3);
    utils.convertStringToULArray(getSettingByName(settingsline,"lamp_profile_4:",' '), lamp_profile_4);
    //Serial.println("profile from settings------- "+lamp_profile_4+"-------------");
  }
}

void FileManagement::applySettings(LampModule* lampmod, TimeModule* timemod, WiFiModule* wifimod) {
  readSettings();
  if (wifi_name != "") {
    wifimod->setSSID(wifi_name);
  }
  if (wifi_password != "") {
    wifimod->setPassword(wifi_password);
  }
  if (fulltime != 0) {
    timemod->setTime(fulltime);
  }
  lampmod->setProfile(1, lamp_profile_1, sizeof(lamp_profile_1) / sizeof(lamp_profile_1[0]));
  lampmod->setProfile(2, lamp_profile_2, sizeof(lamp_profile_2) / sizeof(lamp_profile_2[0]));
  lampmod->setProfile(3, lamp_profile_3, sizeof(lamp_profile_3) / sizeof(lamp_profile_3[0]));
  lampmod->setProfile(4, lamp_profile_4, sizeof(lamp_profile_4) / sizeof(lamp_profile_4[0]));
  lampmod->setProfilesEndTimer();
  lampmod->setSettingsLoaded(true);
}

void FileManagement::setTime(unsigned long newtime) {
  fulltime = newtime;
}

void FileManagement::setWifiName(String newname) {
  wifi_name = newname;
}

void FileManagement::setWifiPassword(String newpass) {
  wifi_password = newpass;
}

void FileManagement::setSerialSpeed(uint32_t newspeed) {
  serial_speed = newspeed;
}

void FileManagement::setLampStateRed(bool state) {
  lamp_red_state = state;
}

void FileManagement::setLampStateGreen(bool state) {
  lamp_green_state = state;
}

void FileManagement::setLampStateBlue(bool state) {
  lamp_blue_state = state;
}

void FileManagement::setLampStateWhite(bool state) {
  lamp_white_state = state;
}

void FileManagement::setLampBrightRed(uint8_t br) {
  lamp_red_bright = br;
}

void FileManagement::setLampBrightGreen(uint8_t br) {
  lamp_green_bright = br;
}

void FileManagement::setLampBrightBlue(uint8_t br) {
  lamp_blue_bright = br;
}

void FileManagement::setLampBrightWhite(uint8_t br) {
  lamp_white_bright = br;
}

void FileManagement::setLampTimerRed(String timer) {
  lamp_red_timer = timer;
}

void FileManagement::setLampTimerGreen(String timer) {
  lamp_green_timer = timer;
}

void FileManagement::setLampTimerBlue(String timer) {
  lamp_blue_timer = timer;
}

void FileManagement::setLampTimerWhite(String timer) {
  lamp_white_timer = timer;
}

void FileManagement::setLampProfile(int profilename, unsigned long* profilemass, uint masssize) {
  Serial.println("name: "+String(profilename));
  Serial.println(profilemass[3]);
  switch (profilename) {
    case 1:
      {
        utils.copyArray(masssize,5,lamp_profile_1,profilemass);
        break;
      }
    case 2:
      {
        utils.copyArray(masssize,5,lamp_profile_2,profilemass);
        break;
      }
    case 3:
      {
        utils.copyArray(masssize,5,lamp_profile_3,profilemass);
        break;
      }
    case 4:
      {
        utils.copyArray(masssize,5,lamp_profile_4,profilemass);
        break;
      }
  }
}

String FileManagement::getSettingByName(String settingsline,String name,char endchar){
  if (settingsline.indexOf(name) != -1) {
        uint16_t indexstart = settingsline.indexOf(name) + name.length();
        uint16_t indexend = settingsline.indexOf(endchar, indexstart) > 0 ? settingsline.indexOf(endchar, indexstart) : settingsline.length();
        return settingsline.substring(indexstart, indexend);
      }
      return "";
}

File FileManagement::getWebFiles(String name) {
  File file = LittleFS.open("/" + name, "r");
  if (!file) {
    Serial.println("file not found");
    return File();
  }
  return file;
}

void FileManagement::saveLoginData(String username,String password){
  File file = LittleFS.open("/sv/login_data.txt", "w");
  if (!file) {
    Serial.println("Failed to open file");
    file = LittleFS.open("/sv/login_data.txt", "w",true);
    if(!file){
      Serial.println("failed to create file");
      return;
    }
  }

}