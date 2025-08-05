#ifndef WIFIMODULE_H
#define WIFIMODULE_H
#include <Arduino.h>
#include <WiFi.h>

class WiFiModule{
public:
  WiFiModule();
  WiFiModule(int portnum,String newssid,String pass,String newhost,uint16_t newport);
  String getSSID();
  String getPassword();
  int getSignalQuality();
  void setSSID(String newssid);
  void setPassword(String newpass);
  void setSetup();
  bool isConnected();
  void Reconnect();
private:
  String ssid;
  String password;
  uint32_t serial_num;
  bool connected;
  const char* ssid_station = "Aquarium controller";
  const char* password_station = "12345678";
};

#endif