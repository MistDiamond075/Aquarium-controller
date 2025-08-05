#define DEBUG_WEBSOCKETS
#include <LiquidCrystal_I2C.h>
#include <WiFi.h>
#include <Arduino.h>
#include <WebSocketsServer.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include "TimeModule.h"
#include "WiFiModule.h"
#include "FileManagement.h"
#include "KeypadControlModule.h"
#include "MenuModule.h"
#include "LampModule.h"
#include "DetectorTemperatureModule.h"
#include "Bitmaps.h"
#include "HttpServer.h"
#include "RusFont.h"

//Lamp controller
LampModule lampmod;

//FS module
FileManagement fmgmt;

//KeypadController
KeypadControlModule keypadmod;

//WiFi module
WiFiModule wifimod(9600,"test.com","137-k","106245ee",17);

//I2C module
const int8_t RST_PIN = 17;
const int8_t CE_PIN = 5;
const int8_t DC_PIN = 19;
//const int8_t DIN_PIN = D7;  // Uncomment for Software SPI
//const int8_t CLK_PIN = D5;  // Uncomment for Software SPI
const int8_t BL_PIN = 2;
Adafruit_PCD8544 lcd = Adafruit_PCD8544(DC_PIN, CE_PIN, RST_PIN);

//Time module
TimeModule timemod(&lampmod);

//Temperature detector module
DetectorTemperatureModule dtempmod;

//HTTP server module
//HttpServer websv;

//WebSocket server module
extern WebSocketsServer WSserver; 
void WSsetSetup(FileManagement* fm, TimeModule* tm, LampModule* lm, WiFiModule* wfm, DetectorTemperatureModule* dtm);

//MenuButtons
MenuModule menumod(&fmgmt,&wifimod,&timemod,&lampmod);

//Misc parametrs
int serial_port_num=57600;
int timer=millis();

void setup(){
  Serial.begin(9600);
  Serial.setTimeout(50);
 // Serial1.begin(57600);
  fmgmt.setSetup();
  Serial.println("settings loaded");
  fmgmt.applySettings(&lampmod, &timemod, &wifimod);
  Serial.println("settings applyed");
  wifimod.setSetup();
  Serial.println("wifi loaded");
  pinMode(BL_PIN, OUTPUT);
  digitalWrite(BL_PIN, HIGH);
   timemod.setSetup(wifimod.isConnected());
  timemod.setConnect(wifimod.isConnected());
  Serial.println("time loaded");
  lcd.begin(); 
  lcd.clearDisplay();
    lcd.setContrast(60);    
    lcd.setTextSize(1);
  lcd.setTextColor(BLACK);
  lcd.drawBitmap(0, 0, thbmp, 88, 48, 1);
  lcd.display();                 
 // delay(3000);
  keypadmod.setSetup();
  Serial.println("keypad loaded");
  dtempmod.setSetup();
  menumod.setSetup();
  Serial.println("menu loaded");
  lcd.clearDisplay();
  lcd.setFont(&Dited1402257pt7b);
  lampmod.setSetup();
  Serial.println("lamp loaded");
  //timemod.TimeEmulation();
  //menumod.changeButton();
  menumod.showButton2(menumod.getBtnProp());
  keypadmod.setTimemodule(&timemod);
  //websv.setSetup(&fmgmt, &timemod, &lampmod, &wifimod);
  WSsetSetup(&fmgmt, &timemod, &lampmod, &wifimod, &dtempmod);
  Serial.println("setup loaded");
  lcd.print("setup loaded");
  delay(3000);
  lcd.clearDisplay();
  unsigned long prof[5];
  lampmod.getProfileInfo(1,prof, 5);
  for(int i=0;i<5;i++){
    Serial.print("mass[i]: ");Serial.println(prof[i]);
  }
  Serial.println("&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&");
  //menumod.getButtonsNames();
}

void loop(){
  timemod.TimeEmulation();
 //websv.RemoteControl();
  WSserver.loop();
  listenerKeypad();
  dtempmod.DetectorState();
  if(menumod.isTimeShowing() || timemod.isSleeping()){
   // menumod.confButton2("p0ka3at` vremo");
  }
  else if(menumod.getAction()==2){
    menumod.Kbenter(menumod.getKbtype());
  }else if(menumod.getAction()==1){
    menumod.showButton2(menumod.getBtnProp());
  }
 // Serial.println(menumod.getMenuState());
  //lampmod.LampEmulation();
    //Serial.println(menumod.getKbpage());
  //Serial.println(menumod.getKbtype());
//Serial.println(fmgmt.getRedState());
}

void listenerKeypad(){
switch(keypadmod.KeypadState()){
    case 9:{  //right
    if(menumod.getAction()==1){
      menumod.setMenuCursorPos(0);
      menumod.setMenuBtnPos(0);
      menumod.setMenuMassPos(menumod.getMenuMassPos()+1);
      menumod.showButton2(menumod.getBtnProp());
      keypadmod.setSwitching(true);
    }
    else if(menumod.getAction()==2){
      menumod.setKbpos(menumod.getKbpos()+1);
      keypadmod.setSwitching(true);
    }
    break;}
    case 1:{  //left
    if(menumod.getAction()==1){
      menumod.setMenuCursorPos(0);
      menumod.setMenuBtnPos(0);
      menumod.setMenuMassPos(menumod.getMenuMassPos()-1);
      menumod.showButton2(menumod.getBtnProp());
      keypadmod.setSwitching(true);
    }
    else if(menumod.getAction()==2){
      menumod.setKbpos(menumod.getKbpos()-1);
      keypadmod.setSwitching(true);
    }
    break;}
    case 6:{  //up
    if(menumod.getAction()==1){
      menumod.setMenuCursorPos(menumod.getMenuCursorPos()-8);
       menumod.setMenuBtnPos(menumod.getMenuBtnPos()-1);
       menumod.showButton2(menumod.getBtnPropMenuForPrint());
      keypadmod.setSwitching(true);
    }
    else if(menumod.getAction()==2){
      menumod.setKbpos(menumod.getKbpos()-14);
      keypadmod.setSwitching(true);
    }
    break;}
    case 4:{  //down
    if(menumod.getAction()==1){
      menumod.setMenuCursorPos(menumod.getMenuCursorPos()+8);
      menumod.setMenuBtnPos(menumod.getMenuBtnPos()+1);
      menumod.showButton2(menumod.getBtnPropMenuForPrint());
      keypadmod.setSwitching(true);
    }
    else if(menumod.getAction()==2){
      menumod.setKbpos(menumod.getKbpos()+14);
      keypadmod.setSwitching(true);
    }
    break;}
    case 5:{  //set symbol
      if(menumod.getAction()==2){
        menumod.Kbsetsym(true);
      }
      keypadmod.setSwitching(true);
    break;}
    case 11:{  //confirm
      if(menumod.getAction()==2){
        menumod.setKbconf(true);
      }
      else if(menumod.getAction()==1){
        menumod.setMenuCursorPos(0);
       // Serial.println("conf btn prop: "+menumod.getBtnProp());
       menumod.confButton2(menumod.getBtnProp());
      }
      keypadmod.setSwitching(true);
    break;}
    case 8:{  //switch letters size on keyboard
    Serial.println("Pressed 8");
      if(menumod.getAction()==2){
        menumod.setKbshift();
      }
      keypadmod.setSwitching(true);
    break;}
    case 3:{  //back
      if(menumod.getAction()==2){
        menumod.setKbdeny(true);
        Serial.println("denied");
      }
      else if(menumod.getAction()==1 || menumod.getAction()==3){
        if(menumod.getAction()==3){
          menumod.setAction(1);
        }
        menumod.setMenuCursorPos(0);
        menumod.setMenuBtnPos(0);
        //Serial.println("menumass pos back: "+String(menumod.getMenuMassPos()));
        if(menumod.getMenuMassPos()==3){
          menumod.setMenuMassPos(menumod.getMenuMassPos()-1);
        }else if(menumod.getMenuMassPos()>3){
          menumod.menumasspos=menumod.getMenuMassPos()-1;
        }
      menumod.showButton2(menumod.getBtnPropMenuForPrint());
        menumod.showButton2(menumod.getBtnProp());
      }
      keypadmod.setSwitching(true);
    break;}
    case 7:{
    /* Serial.println("1== "+lampmod.getProfileInfo(1));
      Serial.println("2== "+lampmod.getProfileInfo(2));
      Serial.println("3== "+lampmod.getProfileInfo(3));
      Serial.println("4== "+lampmod.getProfileInfo(4));*/
      /*lampmod.setWhiteBright(lampmod.getWhiteBright()+20);
      lampmod.loadProfile(2);*/
      /*lcd.clearDisplay();
      lcd.drawBitmap(0, 0, thbmp, 88, 48, 1);
      lcd.display(); 
      Serial.println(String(menumod.getLampProfile()));*/
      keypadmod.setSwitching(true);
    break;}
    case 0:{  //delete symbol
      menumod.setKbdel(true);
      keypadmod.setSwitching(true);
    break;}
    case -1:{break;}
  }
}