#include "HttpServer.h"
#include <Arduino.h>
#include <WebServer.h>
#include "WiFiModule.h"
#include <LittleFS.h>
#include "ctype.h"

HttpServer::HttpServer()
  : server(sv_port), svusername("admin"), svpassword("admin") {
}

void HttpServer::setSetup(FileManagement* fm, TimeModule* tm, LampModule* lm, WiFiModule* wfm) {
  Serial.begin(9600);
  fmgmt = fm;
  lampmod = lm;
  timemod = tm;
  wifimod = wfm;
  if (wifimod->isConnected()) {
    server.on("/", [this]() {
      handleRoot();
    });
    server.on("/loginpage", [this]() {
      handleLoginpage();
    });
    server.on("/getServerInfo", [this]() {
      getServerInfo();
    });
    server.on("/style.css", [this]() {
      getCss(server.uri());
    });
    server.on("/main.js", [this]() {
      getJs(server.uri());
    });
    server.on("/getTime",[this](){
      getTime();
    });
    server.on("/updTime", [this]() {
      updateTime();
    });
    server.on("/updWifi", [this]() {
      updateWifi();
    });
    server.on("/updProfile", [this]() {
      updateProfile();
    });
    server.on("/updUser", [this]() {
      updateUser();
    });
    server.on("/login", [this]() {
      handleAuth();
    });
    server.on("/logout", [this]() {
      handleUnauth();
    });
    server.onNotFound([this]() {
      getNonExistentPage();
    });
    server.begin(sv_port);
    Serial.println("server started");
  } else {
    Serial.println("server not started: no internet connection");
  }
}

void HttpServer::RemoteControl() {
  server.handleClient();
}

void HttpServer::handleRoot() {
  if (!checkAuth(server)) {
    handleLoginpage();
    return;
  }
  Serial.println("get root");
  File file = fmgmt->getWebFiles("index.html");
  //server.send(200,"text/plain","server work");
  server.streamFile(file, "text/html");
  file.close();
}

void HttpServer::handleLoginpage() {
  File file = fmgmt->getWebFiles("loginpage.html");
  //server.send(200,"text/plain","server work");
  server.streamFile(file, "text/html");
  file.close();
}

void HttpServer::getCss(String path) {
  File file = fmgmt->getWebFiles(path);
  server.streamFile(file, "text/css");
  file.close();
}

void HttpServer::getJs(String path) {
  File file = fmgmt->getWebFiles(path);
  server.streamFile(file, "text/javascript");
  file.close();
}

void HttpServer::getServerInfo() {
  if (!checkAuth(server)) {
    handleLoginpage();
    return;
  }
  String senddata = "";
  //String lampinfo[4];
  uint8_t iterator = 0;
  unsigned long profinfo[5];
  for (int i = 1; i < 5; i++) {
    lampmod->getProfileInfo(i, profinfo, sizeof(profinfo) / sizeof(profinfo[0]));
    senddata += "Profile " + String(i) + " [" + "red: " + String(profinfo[0]) + "],[green: " + String(profinfo[1]) + "],[blue: " + String(profinfo[2]) + "],[white: " + String(profinfo[3]) + "],[starts from: " + utils.convertULTimerToTime(profinfo[4]) + ']';
    senddata += '\n';
  }
  senddata += "Wifi [ssid: " + wifimod->getPassword() + "],[";
  senddata += "password: " + wifimod->getSSID() + "]\n";
  senddata += timemod->getTime()+'\n';
  senddata += "username: " + svusername+'\n';
  senddata += "password: " + svpassword;
  server.send(200, "text/plain", senddata);
}

void HttpServer::getTime(){
  server.send(200,"text/plain",timemod->getTime());
}

void HttpServer::getNonExistentPage() {
  String path = server.uri();
  if (path.endsWith(".css")) {
    getCss(path);
  } else if (path.endsWith(".js")) {
    getJs(path);
  } else {
    server.send(404, "text/plain", "page not found");
  }
}

WebServer& HttpServer::getServer() {
  return server;
}

void HttpServer::updateTime() {
  Serial.println(checkAuth(server));
  if (!checkAuth(server)) {
    handleLoginpage();
    return;
  }
  Serial.print("arg: ");
  Serial.println(server.hasArg("plain"));
  uint8_t hours = server.hasArg("hours") ? server.arg("hours").toInt() : 25;
  uint8_t minutes = server.hasArg("minutes") ? server.arg("minutes").toInt() : 61;
  uint8_t seconds = server.hasArg("seconds") ? server.arg("seconds").toInt() : 61;
  if (hours > 24 || minutes > 60 || seconds > 60) {
    server.send(400, "request params error");
    return;
  }
  unsigned long ultime = 0;
  ultime = hours * 3600 + minutes * 60 + seconds;
  Serial.print("h:");
  Serial.println(hours);
  Serial.print("m:");
  Serial.println(minutes);
  Serial.print("time: ");
  Serial.println(ultime);
  timemod->setTime(ultime);
  fmgmt->setTime(ultime);
  fmgmt->writeSettings();
  server.send(200, "time updated");
}

void HttpServer::updateWifi() {
  if (!checkAuth(server)) {
    handleLoginpage();
    return;
  }
  if (!server.hasArg("plain")) {
    Serial.println("No Data Received!");
    server.send(400, "No data received");
    return;
  }

  String rawData = server.arg("plain");

  JsonDocument doc;
  DeserializationError error = deserializeMsgPack(doc, (uint8_t*)rawData.c_str(), rawData.length());
  if (!error) {
    String ssid = doc["ssid"];
    String pass = doc["password"];
    if (ssid != "") {
      Serial.println("ssid: " + ssid);
      wifimod->setSSID(ssid);
      fmgmt->setWifiName(ssid);
      fmgmt->writeSettings();
    }
    if (pass != "") {
      Serial.println("pass: " + pass);
      wifimod->setPassword(pass);
      fmgmt->setWifiPassword(pass);
      fmgmt->writeSettings();
    }
    server.send(200, "wifi updated");
  } else {
    server.send(400, "request with incorrect body");
  }
}

void HttpServer::updateProfile() {
  if (!checkAuth(server)) {
    handleLoginpage();
    return;
  }
  if (server.hasArg("profile")) {

    auto setProfileData = [this](unsigned long* prof, uint profsize, bool allcolor, String newbright, String newtimer, String newcolor, int profnum) {
      lampmod->getProfileInfo(profnum, prof, profsize);
      if (prof == nullptr) {
        server.send(400, "getting profile info error");
        return;
      }
      if (utils.isStringNumber(newbright)) {
        if (!allcolor) {
          if (newcolor == "red") {
            prof[0] = newbright.toInt();
          } else if (newcolor == "green") {
            prof[1] = newbright.toInt();
          } else if (newcolor == "blue") {
            prof[2] = newbright.toInt();
          } else if (newcolor == "white") {
            prof[3] = newbright.toInt();
          }
        } else {
          for (int i = 0; i < 4; i++) {
            prof[i] = newbright.toInt();
          }
        }
      }
      if (utils.isStringNumber(newtimer)) {
        Serial.print("newtimer to append: ");
        Serial.println(newtimer.toInt());
        prof[4] = newtimer.toInt();
      }
      Serial.print("profnum: ");
      Serial.println(profnum);
      lampmod->setProfile(profnum, prof, profsize);
      fmgmt->setLampProfile(profnum, prof, profsize);
      fmgmt->writeSettings();
    };

    unsigned long prof[5];
    String strprofnum = server.arg("profile");
    String strcolor = server.hasArg("color") ? server.arg("color") : "";
    String newbright = server.hasArg("bright") ? server.arg("bright") : "";
    String newtimer = server.hasArg("timer") ? server.arg("timer") : "";
    Serial.println(strprofnum);
    Serial.println(strcolor);
    Serial.println(newbright);
    Serial.println(newtimer);
    if (newtimer != "" && newtimer.indexOf(':') != -1) {
      String h = newtimer.substring(0, newtimer.indexOf(':'));
      if (newtimer.indexOf(':') != newtimer.length() - 1) {
        String m = newtimer.substring(newtimer.indexOf(':') + 1);
        if (utils.isStringNumber(h) && utils.isStringNumber(m)) {
          unsigned long fulltime = h.toInt() * 3600 + m.toInt() * 60;
          newtimer = String(fulltime);
        }
      }
    }
    Serial.print("newtimer: ");
    Serial.println(newtimer);
    bool allcolor = (strcolor == "all");
    bool allprofile = (strprofnum == "all");
    Serial.println(allcolor);
    Serial.println(allprofile);
    if (!allprofile) {
      int8_t profnum = utils.isStringNumber(strprofnum.substring(strprofnum.length() - 1)) ? strprofnum.substring(strprofnum.length() - 1).toInt() : 0;
      setProfileData(prof, sizeof(prof) / sizeof(prof[0]), allcolor, newbright, newtimer, strcolor, profnum);
    } else {
      for (int i = 1; i < lampmod->getProfileCount() + 1; i++) {
        setProfileData(prof, sizeof(prof) / sizeof(prof[0]), allcolor, newbright, newtimer, strcolor, i);
      }
    }
    server.send(200, "profile updated");
  }
  server.send(400, "request with incorrect body");
}

void HttpServer::updateUser() {
  if (!server.hasArg("plain")) {
    Serial.println("No Data Received!");
    server.send(400, "No data received");
    return;
  }

  String rawData = server.arg("plain");

  JsonDocument doc;
  DeserializationError error = deserializeMsgPack(doc, (uint8_t*)rawData.c_str(), rawData.length());
  if (!error) {
    String username = doc["username"];
    String password = doc["password"];
    Serial.println(username);
    Serial.println(password);
    if (username != "" && password != "") {
      svusername = username;
      svpassword = password;
      server.send(200, "user data updated succesfully");
    }
  } else {
    server.send(400, "wrong data");
  }
}

void HttpServer::handleAuth() {
  if (!server.hasArg("plain")) {
    Serial.println("No Data Received!");
    server.send(400, "No data received");
    return;
  }

  String rawData = server.arg("plain");

  JsonDocument doc;
  DeserializationError error = deserializeMsgPack(doc, (uint8_t*)rawData.c_str(), rawData.length());

  Serial.println(error.c_str());
  if (!error) {
    String username = doc["username"];
    String password = doc["password"];
    Serial.println(username);
    Serial.println(password);
    if (username == svusername && password == svpassword) {
      server.send(200, createJWT(username).c_str());
    }
  } else {
    server.send(401, "wrong login data");
  }
}

void HttpServer::handleUnauth() {
  server.send(200, "logged out");
}

String HttpServer::createJWT(String username) {
  JsonDocument doc;
  doc["username"] = username;
  doc["exp"] = millis() + 3600000;
  String payload;
  serializeJson(doc, payload);
  String encoded_payload = base64Encode(payload);
  String signature = base64Encode(JWT_KEY);
  return encoded_payload + "." + signature;
}

bool HttpServer::checkJWT(String jwt) {
  if (jwt.indexOf('.') == -1) return false;
  String payload = jwt.substring(0, jwt.indexOf('.'));
  String signature = jwt.substring(jwt.indexOf('.') + 1);
  String decoded_payload = base64Decode(payload);
  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, decoded_payload);
  if (error) return false;
  unsigned long exp = doc["exp"];
  if (millis() > exp) return false;
  return (signature == base64Decode(JWT_KEY));
}

String HttpServer::base64Encode(const String& input) {
  size_t outputLen;
  unsigned char output[128];
  mbedtls_base64_encode(output, sizeof(output), &outputLen, (const unsigned char*)input.c_str(), input.length());
  return String((char*)output);
}

String HttpServer::base64Decode(const String& input) {
  size_t outputLen;
  unsigned char output[128];
  mbedtls_base64_decode(output, sizeof(output), &outputLen, (const unsigned char*)input.c_str(), input.length());
  return String((char*)output);
}

bool HttpServer::checkAuth(WebServer& sv) {
  if (!sv.hasHeader("Authorization")) {
    return false;
  }
  return true;
}

bool HttpServer::matchTimePattern(const String& time, DateTimePattern pattern) {
  switch (pattern) {
    case DateTimePattern::HHMM:
      {
        if (time.length() != 5) {
          return false;
        }
        if (time.indexOf(':') == -1 || time.indexOf(':') == time.length() - 1) {
          return false;
        }
        String h = time.substring(0, time.indexOf(':'));
        String m = time.substring(time.indexOf(':') + 1);
        if (utils.isStringNumber(h) && utils.isStringNumber(m)) {
          return true;
        }
        break;
      }
    case DateTimePattern::HHMMSS:
      {
        if (time.length() != 8) {
          return false;
        }
        if (time.indexOf(':') == -1 || time.indexOf(':') == time.length() - 1) {
          return false;
        }
        uint delimitercount = 0;
        for (int i = 0; i < time.length(); i++) {
          if (time.charAt(i) == ':') {
            delimitercount++;
          }
        }
        Serial.println(delimitercount);
        if (delimitercount != 2) {
          return false;
        }
        String h = time.substring(0, time.indexOf(':'));
        String m = time.substring(3, time.indexOf(':', time.indexOf(':') + 1));
        String s = time.substring(time.lastIndexOf(':') + 1);
        Serial.println(String(h) + " " + String(m) + " " + String(s));
        if (utils.isStringNumber(h) && utils.isStringNumber(m) && utils.isStringNumber(s)) {
          Serial.println("pattern true");
          return true;
        }
        break;
      }
  }
  Serial.println("pattern false");
  return false;
}