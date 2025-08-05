#include "WiFiModule.h"
#include <Arduino.h>
#include <WiFi.h>

WiFiModule::WiFiModule():serial_num(9600),ssid("137-k"),password("106245ee"),connected(false){

}

WiFiModule::WiFiModule(int num,String newhost,String newssid,String pass,uint16_t newport){
  serial_num=num;
  ssid=newssid;
  password=pass;
  connected=false;
}

String WiFiModule::getSSID(){
  return ssid;
}

String WiFiModule::getPassword(){
  return password;
}

int WiFiModule::getSignalQuality() {
  long rssi = WiFi.RSSI();
  if (rssi <= -100) return 0;
  else if (rssi >= -50) return 100;
  else return 2 * (rssi + 100);
}

bool WiFiModule::isConnected(){
  return connected;
}

void WiFiModule::setSetup(){
  Serial.begin(serial_num);
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  //WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  //WiFi.softAP(ssid_station,password_station);
  //IPAddress IP = WiFi.softAPIP();
  //Serial.print("Access Point IP: ");
  //Serial.println(IP);
  String loading=".";
  connected=true;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(loading);
    loading+=".";
    if(loading.length()>20){Serial.println(loading.length());connected=false;break;}
  }
  Serial.println("");
  if(connected){
    Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  }
}

void WiFiModule::setSSID(String newssid){
  ssid=newssid;
}

void WiFiModule::setPassword(String newpass){
  password=newpass;
}

void WiFiModule::Reconnect(){
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}