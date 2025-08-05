#include <Arduino.h>
#include <climits>
#include "Utils.h"

Utils::Utils(){

}

String Utils::convertULArrayToString(unsigned long* massive, uint masssize) {
  String str_ret = "";
  Serial.println("mass_size: "+String(masssize));
  for (int i = 0; i < masssize; i++) {
    str_ret += massive[i];
    Serial.println(massive[i]);
    if (i != masssize - 1) { str_ret += ","; }
  }
  Serial.print("writing mass to settings: ");
  Serial.println(str_ret);
  return str_ret;
}

void Utils::convertStringToULArray(String strmassive, unsigned long* mass) {
  uint8_t mass_size = 0;
  for (int i = 0; i < strmassive.length(); i++) {
    if (strmassive[i] == ',') { mass_size++; }
  }
  if (strmassive.length() == 1) {
    mass_size = 1;
  } else {
    mass_size++;
  }
  if (mass_size == 0) {
    return;
  }
  // Serial.print("mass size: ");Serial.println(mass_size);
  //Serial.println("String to convert: "+strmassive);
  //Serial.println("String to mass: ");
  for (int i = 0; i < mass_size; i++) {
    //Serial.println(strmassive.indexOf(','));
    mass[i] = strmassive.substring(0, strmassive.indexOf(',')).toInt();
    //Serial.print("mass[i]: ");Serial.println(strmassive.substring(0,strmassive.indexOf(',')));
    if (i != mass_size - 1) { strmassive = strmassive.substring(strmassive.indexOf(',') + 1); }
  }
}

unsigned long Utils::convertStringTimerToULTime(String strtimer) {
  if (strtimer.length() != 6) {
    Serial.println("Invalid timer format");
    return ULONG_MAX;
  }
  String h = "";
  String m = "";
  String s = "";
  unsigned long timeret = ULONG_MAX;
  h = strtimer.substring(0, 2);
  strtimer = strtimer.substring(2);
  m = strtimer.substring(0, 2);
  strtimer = strtimer.substring(2);
  s = strtimer.substring(0);
  timeret = h.toInt() * 3600 + m.toInt() * 60 + s.toInt();
  return timeret;
}

bool Utils::convertStringToBoolean(String str){
  if(str=="1" || str=="true"){
    return true;
  }
  return false;
}

String Utils::convertULTimerToTime(unsigned long timer){
  int seconds = timer % 60;
  int minutes = (timer / 60) % 60;
  int hours = (timer / 3600) % 24;
  String str_ret=String(hours)+":"+String(minutes)+":"+String(seconds);
  return str_ret;
}

bool Utils::isStringNumber(String num){
  bool is_number=true;
  for(int i=0;i<num.length();i++){
    if(!isDigit(num.charAt(i))){
      is_number=false;
    }
  }
  return is_number;
}

const char* Utils::substrchr(const char* array,uint start){
  return array+start;
}

const char* Utils::substrchr(const char* array,uint start,uint end){
  uint len = strlen(array);  
  if(start>=len || end>=len){
    return nullptr;
  }
  uint substr_len = end - start;
  char* res = new char[substr_len + 1]; 
  strncpy(res, array + start, substr_len);  
  res[substr_len] = '\0';  
  return res;
}

int Utils::indexOfchr(const char* array,char ch){
  const char* pos=strchr(array,ch);
  return (pos) ? pos-array : -1;
}