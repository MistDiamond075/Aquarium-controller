#ifndef MENUMODULE_H
#define MENUMODULE_H
#include "TimeModule.h"
#include "WiFiModule.h"
#include "FileManagement.h"
#include "LampModule.h"
#include "Utils.h"
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>

class MenuModule{
public:
  MenuModule(FileManagement* newfm,WiFiModule* newwfmod,TimeModule* newtm,LampModule* lmp);
  ~MenuModule();
  void setSetup();
  void addButton(String btnproperties,int btnmass);
  void setMenuState(uint16_t newstate);
  bool isTimeShowing();
  uint8_t getAction();
  void setAction(uint8_t act);
  void setKbdel(bool isdel);
  void setKbconf(bool isconf);
  void Kbsetsym(bool conf);
  void setKbpos(uint8_t newpos);
  uint8_t getKbpos();
  void Kbenter(String entertype);
  String getKbtype();
  void setKbshift();
  void setKbdeny(bool deny);
  bool getKbdeny();
  uint8_t getMenuCursorPos();
  void setMenuCursorPos(int8_t newpos);
  uint8_t getLampProfile();
  void showButton2(String btnprop);
  void confButton2(String btnprop);
  void setMenuBtnPos(uint newpos);
  void setMenuMassPos(uint newpos);
  uint getMenuBtnPos();
  uint getMenuMassPos();
  void switchMass(uint nextmass);
  String getBtnProp();
  String getBtnPropMenuForPrint();
  uint menumasspos;
private:
  uint menubtnpos;      //when menu changes, menupos=+1/-1,when kbconfirm => changeButton(getCurrentMenuMass(menupos)) => currentmenumass=newmenumass  
  uint menulevel;
  uint menuforprint;
  const char** currentmenumass;
  uint allmenumasscnt;
  uint currentmenumasscnt;
  const char* timemenumass[2];
  uint timemenumasscnt=2;
  const char* networkmenumass[2];
  uint networkmenumasscnt=2;
  const char* lampmenumass[4];
  uint lampmenumasscnt=4;
  const char* profilemenumass[6];
  uint profilemenumasscnt=6; 
  Utils utils;
  FileManagement* filemgmt;
  WiFiModule* wifimod;
  TimeModule* timemod;
  LampModule* lampmod;
  //LiquidCrystal_I2C lcd;
  const int8_t RST_PIN = 17;
const int8_t CE_PIN = 5;
const int8_t DC_PIN = 19;
//const int8_t DIN_PIN = D7;  // Uncomment for Software SPI
//const int8_t CLK_PIN = D5;  // Uncomment for Software SPI
const int8_t BL_PIN = 2;
Adafruit_PCD8544 lcd;
  bool showingtime;
  uint8_t action;  //1 - scrolling menu, 2 - keyboard
  String kbtype;
  char keyboardLow[27]={' ','a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z'};
  char keyboardHigh[27]={' ','A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z'};
  char keyboardSymbols[37]={'0','1','2','3','4','5','6','7','8','9','!','@','#','%','$','^','&','*','(',')','-','_','+','=',';',':','<','>',',','.','?','/','[',']','{','}','~'};
  bool kbdeny;
  bool kbconfirm;
  bool kbdel;
  bool kbsetsym;
  uint8_t kbpos;
  String newwifiname;
  String newwifipass;
  bool waswifinameset;
  unsigned long newtime;
  String newtimedisplay;
  uint hours;
  uint minutes;
  uint seconds;
  String profiletimestart;
  String profiletimeend;
  bool profiletimestartset;
  String lampbrightvalue;
  String kbeventcolor;
  int8_t lampprofile; 
  int kbeventnumplace;
  int8_t kbshift;
  uint8_t menucursorpos;
  uint8_t menupoints;
  void displayKeyboard();
  void displayInputValues();
  //BUTTON NAMES
  const char btnShowTime[15];
  const char btnSetTime[13];
  const char btnSetWifi[12];
  const char btnGetWifi[10];
  const char btnProfile1[10];
  const char btnProfile2[10];
  const char btnProfile3[10];
  const char btnProfile4[10];
  const char btnProfileBrightRed[10];
  const char btnProfileBrightGreen[10];
  const char btnProfileBrightBlue[10];
  const char btnProfileBrightWhite[10];
  const char btnProfileTimer[14];
  const char btnProfileInfo[13];
};

#endif