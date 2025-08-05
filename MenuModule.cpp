#include "MenuModule.h"
#include "TimeModule.h"
#include "WiFiModule.h"
#include "FileManagement.h"
#include <Arduino.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>
#include "RusFont.h"

MenuModule::~MenuModule() {
  if (currentmenumass) {
    delete[] currentmenumass;
    currentmenumass = nullptr;
  }
}

MenuModule::MenuModule(FileManagement* newfm, WiFiModule* newwfmod, TimeModule* newtm, LampModule* lmp)
  : newtime(0), kbdeny(false),
    showingtime(false), lampmod(lmp), filemgmt(newfm), wifimod(newwfmod), timemod(newtm), action(1), kbconfirm(false), kbdel(false),
    kbsetsym(false), kbpos(0), waswifinameset(false), newtimedisplay(""), hours(61), minutes(61), seconds(61), kbeventcolor(""), lampbrightvalue(""),
    kbeventnumplace(0), lampprofile(-1), lcd(DC_PIN, CE_PIN, RST_PIN), kbshift(0),
    menucursorpos(0), menupoints(0), profiletimestart(""), profiletimeend(""), profiletimestartset(false), timemenumasscnt(2), networkmenumasscnt(2),
    menumasspos(0), lampmenumasscnt(4), profilemenumasscnt(6), menubtnpos(0), menuforprint(0), allmenumasscnt(5), currentmenumasscnt(10),
    btnShowTime("p0ka3at` vremo"),btnSetTime("3adat` vremo"),btnSetWifi("3adat` wifi"),btnGetWifi("wifi inf0"),btnProfile1("Pr0fil` 1"),
    btnProfile2("Pr0fil` 2"),btnProfile3("Pr0fil` 3"),btnProfile4("Pr0fil` 4"),btnProfileBrightRed("ork0st` k"),btnProfileBrightGreen("ork0st` 3"),
    btnProfileBrightBlue("ork0st` s"),btnProfileBrightWhite("ork0st` 6"),btnProfileTimer("3adat` taimer"),btnProfileInfo("Pr0fil` inf0"),
    timemenumass{btnShowTime,btnSetTime},networkmenumass{btnGetWifi,btnSetWifi},lampmenumass{btnProfile1,btnProfile2,btnProfile3,btnProfile4},
    profilemenumass{btnProfileBrightRed,btnProfileBrightGreen,btnProfileBrightBlue,btnProfileBrightWhite,btnProfileTimer,btnProfileInfo} {
}

void MenuModule::setSetup() {
  Serial.begin(9600);
  Serial.println("SETUPSETUPSETUP========================");
  currentmenumass = new const char*[currentmenumasscnt];
  if (!networkmenumass) {
    Serial.println("creating network menu error");
  } else if (!timemenumass) {
    Serial.println("creating time menu error");
  } else if (!profilemenumass) {
    Serial.println("creating profile menu error");
  } else if (!currentmenumass) {
    Serial.println("creating current menu error");
  } else if (!lampmenumass) {
    Serial.println("creating lamp menu error");
  }
  for (int i = 0; i < timemenumasscnt; i++) {
    currentmenumass[i] = timemenumass[i];
  }
  currentmenumasscnt=menupoints = timemenumasscnt;
  lcd.setFont(&Dited1402257pt7b);
}

void MenuModule::showButton2(String btnprop) {
  showingtime = false;
  //Serial.println("currentmenucnt: " + String(currentmenumasscnt));
  //Serial.println(getBtnProp());
  int lcdpos = 7;
  //Serial.println("----------------------------");
  lcd.clearDisplay();
  lcd.println("");
  unsigned long currentMillis = millis();
  
 // Serial.println("cnt: "+String(currentmenumasscnt));
  for (int i = 0; i < currentmenumasscnt; i++) {
    lcd.setCursor(0, lcdpos);
    //Serial.println("["+String(i)+"] "+currentmenumass[i]);
    if (i == menucursorpos / 8) {
        lcd.fillRect(0,lcdpos-7 , lcd.width(), 9, BLACK);  // Черный фон
        lcd.setTextColor(WHITE);
        //lcd.setCursor(0, lcdpos);
        lcd.print(currentmenumass[i]);
    } else {
      lcd.setTextColor(BLACK);
      lcd.print(currentmenumass[i]); // Остальные строки выводим без изменений
    }
    lcdpos += 8;
  }
  //Serial.println("menucursorpos: "+String(menucursorpos));
  lcd.display();
}

void MenuModule::confButton2(String btnprop) {
  showingtime = false;
  lcd.setTextColor(BLACK);
  Serial.println("btnprop: "+btnprop);
  const char* strbright=utils.substrchr(btnProfile1,0,utils.indexOfchr(btnProfile1,' '));
  const char* strprof=utils.substrchr(btnProfile1,0,utils.indexOfchr(btnProfile1,' '));
  if (btnprop.indexOf(btnShowTime) != -1) {
    //Serial.println("/////////");
    lcd.clearDisplay();
    lcd.setCursor(0, 8);
    //lcd.print("");
    //lcd.println("Vremo");
    //lcd.setCursor(0,12);
    lcd.println(timemod->getTime());
    //lcd.setCursor(0,24);
    lcd.print(timemod->getDate());
    lcd.display();
    showingtime = true;
    setMenuBtnPos(0);
  } else if (btnprop.indexOf(btnGetWifi) != -1) {
    action=3;
    lcd.clearDisplay();
    lcd.setCursor(0, 8);
    lcd.print("Set`:");
    lcd.setCursor(0, 17);
    lcd.print(wifimod->getSSID());
    lcd.setCursor(0, 26);
    lcd.print("Par0l`:");
    lcd.setCursor(0, 34);
    lcd.print(wifimod->getPassword());
    lcd.display();
    setMenuBtnPos(0);
  } else if (btnprop.indexOf(btnProfileInfo) != -1) {
    action=3;
    lcd.clearDisplay();
    //lampmod->loadProfile(lampprofile);
    unsigned long profileprop[5];
    lampmod->getProfileInfo(lampprofile, profileprop, sizeof(profileprop) / sizeof(profileprop[0]));
    Serial.println(lampprofile);
    String rbright = profileprop[0]<=100 ? String(profileprop[0]) : "vbkl";
    String gbright = profileprop[1]<=100 ? String(profileprop[1]) : "vbkl";
    String bbright = profileprop[2]<=100 ? String(profileprop[2]) : "vbkl";
    String wbright = profileprop[3]<=100 ? String(profileprop[3]) : "vbkl";
    String proftime = profileprop[4]<=100 ? String(profileprop[4]) : "vbkl";
    Serial.println(profileprop[0]<=100 ? String(profileprop[0]) : "vbkl");
    Serial.println(profileprop[1]<=100 ? String(profileprop[1]) : "vbkl");
    Serial.println("R: "+String(profileprop[0]));
    lcd.setCursor(0, 7);
    lcd.println("kpacnbi:" + rbright);
    lcd.setCursor(0, 15);
    lcd.println("3elenbi:" + gbright);
    lcd.setCursor(0, 23);
    lcd.println("cinii:" + bbright);
    lcd.setCursor(0, 31);
    lcd.println("6elbi:" + wbright);
    lcd.setCursor(0, 39);
    lcd.print("taimer:" + proftime);
    lcd.display();
    setMenuBtnPos(0);
    menumasspos = 4;
  }
  else if (btnprop.indexOf(strprof) != -1) {
    String prof = btnprop.substring(btnprop.indexOf(' '));
    lampprofile = prof.toInt();
    Serial.println("lampprofile: " + String(lampprofile));
    //setMenuMassPos(getMenuMassPos()+1);
    setMenuBtnPos(0);
    menumasspos = 3;
    Serial.println("send conf btn prop: " + getBtnProp());
    switchMass(3);
    showButton2(getBtnProp());
  }
  else if (btnprop.indexOf(btnSetWifi, 0) != -1) {
    newwifiname = "";
    newwifipass = "";
    waswifinameset = false;
    action = 2;
    lcd.setFont();
    kbtype = "kbtextevent:" + btnprop;
    lcd.clearDisplay();
  } else if (btnprop.indexOf(btnSetTime, 0) != -1) {
    action = 2;
    lcd.setFont();
    kbtype= "kbnumberevent:" + btnprop;
    lcd.clearDisplay();
  } else if (btnprop.indexOf(strbright) != -1) {
    action = 2;
    lcd.setFont();
    String tmp = "kbnumberevent:" + btnprop;
    String color = btnprop.substring(btnprop.length() - 1, btnprop.length());
    tmp.remove(tmp.length() - 1, tmp.length());
    kbtype = tmp+color;
    lcd.clearDisplay();
  } else if (btnprop.indexOf(btnProfileTimer) != -1) {
    lcd.clearDisplay();
    action = 2;
    lcd.setFont();
    kbtype = "kbnumberevent:" + btnprop;
  }
  delete [] strbright;
  delete [] strprof;
}

void MenuModule::displayKeyboard(){
  const char* strbright=utils.substrchr(btnProfile1,0,utils.indexOfchr(btnProfile1,' '));
  uint pos_x=(kbpos % 14) * 6;
  uint pos_y=20+7*(kbpos/14);
  char selected_symbol=' ';
  if (kbtype.indexOf("kbnumberevent") != -1) {
    lcd.setCursor(0, 20);
    for (int i = 0; i < 10; i++) {
      lcd.print(keyboardSymbols[i]);
    }
    if (kbtype.indexOf(strbright) != -1) {
     // Serial.println("kbtype: "+kbtype);
      kbeventcolor = kbtype.substring(kbtype.indexOf(' ')+1);
      kbeventnumplace = kbeventcolor.length() + 7;
      lcd.setCursor(0, 0);
      lcd.print("Color:" + kbeventcolor);
    } else if (kbtype.indexOf(btnProfileTimer) != -1) {
      lcd.setCursor(0, 0);
      if (!profiletimestartset) {
        lcd.print("Start:");
      } else {
        lcd.print("End:");
      }
    }
    lcd.display();
  } else if (kbtype.indexOf("kbtextevent") != -1) {
    //lcd.clearDisplay();
    lcd.setCursor(0, 20);
    //Serial.println("pos: "+String(kbpos));
    for (int i = 0; i < 10; i++) {
      if(i==kbpos){
          selected_symbol=keyboardSymbols[i];
        lcd.print(' ');
      }else{
        lcd.print(keyboardSymbols[i]);
      }
    }
    lcd.setCursor(0, 27);
    int ival = 0;
    int iend = 20;
    if (kbshift == 2) {
      ival = 10;
      iend = 37;
    }
    for (int i = ival; i < iend; i++) {
      switch (kbshift) {
        case 0:
          {
            if(kbpos>=14 && i==kbpos-14){
              selected_symbol=keyboardLow[i];
              lcd.print(' ');
            }else{
              lcd.print(keyboardLow[i]);
            }
            break;
          }
        case 1:
          {
            if(kbpos>=14 && i==kbpos-14){
              selected_symbol=keyboardHigh[i];
              lcd.print(' ');
            }else{
              lcd.print(keyboardHigh[i]);
            }
            break;
          }
        case 2:
          {
            if(kbpos>=14 && i==kbpos-14){
              selected_symbol=keyboardSymbols[i];
              lcd.print(' ');
            }else{
              lcd.print(keyboardSymbols[i]);
            }
            break;
          }
      }
      /*pos_x+=6;
      if(pos_x==84){
        pos_x=0;
        pos_y+=7;
      }*/
    }
    lcd.display();
  }
  lcd.setCursor(0,20);
    for(int i=0;i<42;i++){
      if(i==kbpos){
        /*Serial.println("x: "+String(pos_x));
        Serial.println("y: "+String(pos_y));
        Serial.println("kbpos: "+String(kbpos));
        Serial.println(kbpos/84);
        Serial.println(kbpos/7);*/
        lcd.fillRect(pos_x,pos_y , 7, 9, BLACK);  // Черный фон
        lcd.setTextColor(WHITE);
        lcd.print(selected_symbol);
        lcd.setTextColor(BLACK);
        break;
      }
      else{lcd.print(" ");}
    }
  //lcd.setCursor(0, 20);
  lcd.display();
  delete [] strbright;
}

void MenuModule::displayInputValues(){
    const char* strbright=utils.substrchr(btnProfile1,0,utils.indexOfchr(btnProfile1,' '));
  if (kbtype.indexOf(btnSetWifi) != -1) {
    if (!waswifinameset) {
      lcd.print(newwifiname);
      // Serial.println(newwifiname);
      lcd.display();
    } else {
      lcd.print(newwifipass);
      lcd.display();
    }
  } else if (kbtype.indexOf(strbright) != -1 && kbeventnumplace != -1) {
    lcd.setCursor(kbeventnumplace, 13);
    lcd.print(lampbrightvalue);
    lcd.display();
  } else if (kbtype.indexOf(btnProfileTimer) != -1) {
    if (!profiletimestartset) {
      lcd.setCursor(0 /*profiletimestart.length()*/, 8);
      lcd.print(profiletimestart);
    } else if (profiletimestartset) {
      lcd.setCursor(0 /*profiletimeend.length()*/, 8);
      lcd.print(profiletimeend);
    }
    lcd.display();
  } else if (kbtype.indexOf(btnSetTime) != -1) {
    if (hours < 24) {
      lcd.print(hours);
      lcd.print(":");
      lcd.display();
    }
    if (minutes < 60) {
      lcd.print(minutes);
      lcd.print(":");
      lcd.display();
    }
    if (seconds < 60) {
      lcd.print(seconds);
      lcd.display();
    }
    lcd.print(newtimedisplay);
    lcd.display();
  }
  delete [] strbright;
}

void MenuModule::Kbenter(String entertype) {
  action = 2;
  unsigned long timer = millis();
  lcd.setCursor(0, 1);
  //Serial.println(kbtype);
  displayKeyboard();
  const char* strbright=utils.substrchr(btnProfile1,0,utils.indexOfchr(btnProfile1,' '));
  if (kbsetsym) {
    if (kbtype.indexOf(btnSetWifi) != -1) {
      if (kbpos < 10) {
        if (!waswifinameset) {
          newwifiname += keyboardSymbols[kbpos];
        } else{
          newwifipass += keyboardSymbols[kbpos];
        }
      } else {
        switch (kbshift) {
          case 0:
            {
              if (!waswifinameset) {
                newwifiname += keyboardLow[kbpos - 14];
              } else{
                newwifipass += keyboardLow[kbpos - 14];
              }
              break;
            }
          case 1:
            {
              if (!waswifinameset) {
                newwifiname += keyboardHigh[kbpos - 14];
              } else{
                newwifipass += keyboardHigh[kbpos - 14];
              }
              break;
            }
          case 2:
            {
              if (!waswifinameset) {
                newwifiname += keyboardSymbols[kbpos - 4];
              } else{
                newwifipass += keyboardSymbols[kbpos - 4];
              }
              break;
            }
        }
      }
      // lcd.display();
    } else if (kbtype.indexOf(btnProfileTimer) != -1) {
      if (!profiletimestartset) {
        profiletimestart += keyboardSymbols[kbpos];
      } else {
        profiletimeend += keyboardSymbols[kbpos];
      }
    } else if (kbtype.indexOf(btnSetTime) != -1) {
      if (kbpos < 11) {
        newtimedisplay += keyboardSymbols[kbpos];
        if (newtimedisplay.length() % 2 == 0) {
          if (hours > 24) {
            hours = newtimedisplay.toInt();
            newtimedisplay = "";
          } else if (minutes > 59) {
            minutes = newtimedisplay.toInt();
            newtimedisplay = "";
          } else if (seconds > 59) {
            seconds = newtimedisplay.toInt();
            newtimedisplay = "";
            //kbconfirm=true;
          }
        }
      }
    } else if (kbtype.indexOf(strbright) != -1) {
      if (kbpos < 11) { lampbrightvalue += keyboardSymbols[kbpos]; }
      // kbconfirm=true;
    }
    kbsetsym = false;
  }
  if (kbdel) {
    auto removeLastSymbol=[] (String& str){
        if(str.length()!=0){
          str.remove(str.length()-1);
        }
      };
    if (kbtype.indexOf(btnSetWifi) != -1) {
      if (!waswifinameset) {
        removeLastSymbol(newwifiname);
      } else {
        removeLastSymbol(newwifipass);
      }
    } else if (kbtype.indexOf(strbright) != -1) {
      if (kbpos < 11) {
        removeLastSymbol(lampbrightvalue);
      }
    } else if (kbtype.indexOf(btnProfileTimer) != -1) {
      if (!profiletimestartset) {
        removeLastSymbol(profiletimestart);
      } else {
        removeLastSymbol(profiletimeend);
      }
    } else if (kbtype.indexOf(btnSetTime) != -1) {
      if (kbpos < 11) {
        removeLastSymbol(newtimedisplay);
      }
    }
    kbdel = false;
    lcd.clearDisplay();
  }
  lcd.setCursor(0, 0);
  displayInputValues();
  if (kbconfirm || kbdeny) {
    if (kbtype.indexOf(btnSetWifi) != -1) {
      if (!waswifinameset) {
        // Serial.println("+ "+newwifiname);
        //newwifipass="";
        lcd.clearDisplay();
        waswifinameset = true;
        kbconfirm = false;
      } else {
        lcd.clearDisplay();
        lcd.setCursor(0, 0);
        //Serial.println("- "+newwifipass);
        if (!kbdeny) {
          wifimod->setSSID(newwifiname);
          wifimod->setPassword(newwifipass);
          filemgmt->setWifiName(newwifiname);
          filemgmt->setWifiPassword(newwifipass);
          filemgmt->writeSettings();
          wifimod->Reconnect();
          if (wifimod->isConnected()) {
            lcd.println("Connected to new wifi");
          } else {
            lcd.println("Connection failed");
          }
        }
        lcd.setFont(&Dited1402257pt7b);
        action = 1;
        kbconfirm = false;
        setMenuCursorPos(0);
        setMenuBtnPos(0);
        showButton2(getBtnPropMenuForPrint());
        kbdeny = false;
        newwifipass="";
        newwifiname="";
        lcd.clearDisplay();
        kbtype = "";
      }
    } else if (kbtype.indexOf(strbright) != -1) {
      if (!kbdeny)
        if (lampbrightvalue.length() == 2 && lampbrightvalue.toInt() <= 100) {
          Serial.println("eventcolor: "+kbeventcolor);
          Serial.println("brval: "+String(lampbrightvalue.toInt()));
          unsigned long newprof[5];
          lampmod->getProfileInfo(lampprofile, newprof, sizeof(newprof) / sizeof(newprof[0]));
          if (kbeventcolor == "k") {
            newprof[0] = lampbrightvalue.toInt();
          } else if (kbeventcolor == "3") {
            newprof[1] = lampbrightvalue.toInt();
          } else if (kbeventcolor == "s") {
            newprof[2] = lampbrightvalue.toInt();
          } else if (kbeventcolor == "6") {
            newprof[3] = lampbrightvalue.toInt();
          }
          Serial.println(newprof[3]);
          lampmod->setProfile(lampprofile, newprof, sizeof(newprof) / sizeof(newprof[0]));
          lampmod->isProfileActive(lampprofile);
          filemgmt->setLampProfile(lampprofile, newprof, sizeof(newprof) / sizeof(newprof[0]));
          //Serial.println("+++++++++++++lampPr0fil` "+String(lampprofile));
          filemgmt->writeSettings();
        }
      if (lampbrightvalue.length() % 2 == 0 || kbdeny) {
        action = 1;
        kbtype = "";
        kbconfirm = false;
        kbsetsym = false;
        lampbrightvalue = "";
        kbeventcolor = "";
        kbeventnumplace = -1;
        setMenuBtnPos(0);
        setMenuCursorPos(0);
        if (!kbdeny)
          filemgmt->writeSettings();
        lcd.setFont(&Dited1402257pt7b);
        kbdeny = false;
        showButton2(getBtnPropMenuForPrint());
      }
    } else if (kbtype.indexOf(btnProfileTimer) != -1) {
      if (profiletimestartset || kbdeny) {
        action = 1;
        kbtype = "";
        kbconfirm = false;
        profiletimestartset = false;
        if (!kbdeny) {
          unsigned long newprof[5];
          lampmod->getProfileInfo(lampprofile, newprof, sizeof(newprof) / sizeof(newprof[0]));
          newprof[4] = utils.convertStringTimerToULTime(profiletimestart);
          //Serial.println("newproffin: "+newproffin);
          lampmod->setProfile(lampprofile, newprof, sizeof(newprof) / sizeof(newprof[0]));
          lampmod->isProfileActive(lampprofile);
          filemgmt->setLampProfile(lampprofile, newprof, sizeof(newprof) / sizeof(newprof[0]));
          filemgmt->writeSettings();
        }
        lcd.setFont(&Dited1402257pt7b);
        kbdeny = false;
        profiletimestart = "";
        profiletimeend = "";
        setMenuBtnPos(0);
        setMenuCursorPos(0);
        showButton2(getBtnPropMenuForPrint());
      } else if (!profiletimestartset) {
        profiletimestartset = true;
        kbconfirm = false;
      }
      lcd.clearDisplay();
    } else if (kbtype.indexOf(btnSetTime) != -1 && (seconds < 60 || kbdeny)) {
      Serial.println("end time entering");
      action = 1;
      setMenuBtnPos(0);
      setMenuCursorPos(0);
      lcd.clearDisplay();
      kbtype = "";
      newtimedisplay = "";
      if (!kbdeny) {
        newtime = hours * 3600 + minutes * 60 + seconds;
        timemod->setTime(newtime);
        filemgmt->setTime(timemod->getPureTime());
      }
      lcd.setFont(&Dited1402257pt7b);
      newtime = 0;
      hours = 61;
      minutes = 61;
      seconds = 61;
      kbdeny = false;
      kbconfirm = false;
      showButton2(getBtnPropMenuForPrint());
    }
  }
  delete [] strbright;
}

bool MenuModule::isTimeShowing() {
  return showingtime;
}

uint8_t MenuModule::getAction() {
  return action;
}

void MenuModule::setAction(uint8_t act){
  action=act;
}

void MenuModule::setKbdel(bool isdel) {
  kbdel = isdel;
}

void MenuModule::setKbconf(bool isconf) {
  kbconfirm = isconf;
}

void MenuModule::Kbsetsym(bool conf) {
  lcd.clearDisplay();
  kbsetsym = conf;
}

uint8_t MenuModule::getKbpos() {
  return kbpos;
}

void MenuModule::setKbpos(uint8_t newpos) {
  Serial.println(newpos);
  lcd.clearDisplay();
  if (kbpos == 14 && newpos == 13) {
    newpos = 9;
  } else if (((kbpos == 9 && newpos == 10) || (newpos >= 10 && newpos <= 13)) && kbtype.indexOf("kbnumberevent") == -1) {
    newpos = 14;
  } else if (newpos >= 10 && kbtype.indexOf("kbnumberevent") != -1) {
    newpos = 9;
  }
  if (newpos >= 254) {
    newpos = 0;
  } else if (newpos > 40) {
    newpos = 40;
  }
  kbpos = newpos;
}

String MenuModule::getKbtype() {
  return kbtype;
}

void MenuModule::setKbshift() {
  lcd.clearDisplay();
  if (kbshift < 2 && kbshift >= 0) {
    kbshift++;
  } else {
    kbshift = 0;
  }
}

bool MenuModule::getKbdeny() {
  return kbdeny;
}

void MenuModule::setKbdeny(bool deny) {
  kbdeny = deny;
}

void MenuModule::setMenuCursorPos(int8_t newpos) {
  // Serial.println("=====before:"+String(menustate));
  Serial.println("newpos: "+String(newpos));
  Serial.println("menupoints: "+String(menupoints));
  if (newpos < 0 || newpos >= UINT8_MAX) {
    newpos = 8 * (menupoints - 1);             //8*menupoints;
  } else if (newpos > 8 * (menupoints - 1)) {  //menupoints){
    newpos = 0;
  }
  //  Serial.println("=====after:"+String(menustate));
  menucursorpos = newpos;
}

uint8_t MenuModule::getMenuCursorPos() {
  return menucursorpos;
}

uint8_t MenuModule::getLampProfile() {
  return lampprofile;
}

void MenuModule::setMenuBtnPos(uint newpos) {
  if (newpos >= currentmenumasscnt && newpos <= UINT_MAX) {
    newpos = 0;
  } else if (newpos >= UINT_MAX) {
    newpos = currentmenumasscnt - 1;
  }
  menubtnpos = newpos;
}

void MenuModule::setMenuMassPos(uint newpos) {
  if (newpos > 2) {
    newpos = 2;
  } else if (newpos < 0) {
    newpos = 0;
  }
  Serial.println("newpos menumass: " + String(newpos));
  menumasspos = newpos;
  switchMass(menumasspos);
}

void MenuModule::switchMass(uint nextmass) {
  Serial.println("nextmass: "+String(nextmass));
  switch (nextmass) {
    case 0:
      {
        currentmenumasscnt=menupoints = timemenumasscnt;
        utils.copyArray(timemenumasscnt,currentmenumasscnt,currentmenumass,timemenumass);
        break;
      }
    case 1:
      {
        currentmenumasscnt=menupoints = networkmenumasscnt;
        utils.copyArray(currentmenumasscnt,currentmenumasscnt,currentmenumass,networkmenumass);
        break;
      }
    case 2:
      {
        currentmenumasscnt=menupoints = lampmenumasscnt;
        utils.copyArray(currentmenumasscnt,currentmenumasscnt,currentmenumass,lampmenumass);
        break;
      }
    case 3:
      {
        currentmenumasscnt=menupoints = profilemenumasscnt;
        utils.copyArray(currentmenumasscnt,currentmenumasscnt,currentmenumass,profilemenumass);
        break;
      }
    default:
      {
        currentmenumasscnt=menupoints = timemenumasscnt;
        utils.copyArray(currentmenumasscnt,currentmenumasscnt,currentmenumass,timemenumass);
      }
  }
}

uint MenuModule::getMenuBtnPos() {
  return menubtnpos;
}

uint MenuModule::getMenuMassPos() {
  return menumasspos;
}

String MenuModule::getBtnProp() {
  return currentmenumass[menubtnpos];
}

String MenuModule::getBtnPropMenuForPrint() {
  return currentmenumass[menuforprint];
}