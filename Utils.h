#ifndef UTILS_H
#define UTILS_H
#include <Arduino.h>

class Utils {
public:
  Utils();
  bool convertStringToBoolean(String str);
  String convertULArrayToString(unsigned long* massive, uint masssize);
  void convertStringToULArray(String strmassive, unsigned long* mass);
  unsigned long convertStringTimerToULTime(String strtimer);
  String convertULTimerToTime(unsigned long timer);  
  bool isStringNumber(String num); 
  const char* substrchr(const char* array,uint start);
  const char* substrchr(const char* array,uint start,uint end);
  int indexOfchr(const char* array,char ch);

  template<typename T> void insertionSort(T* arr, uint size){
    for (int i = 1; i < size; i++) {
      int key = arr[i];
      int j = i - 1;
      while (j >= 0 && arr[j] > key) {
        arr[j + 1] = arr[j];
        j--;
      }
      arr[j + 1] = key;
    }
  }

  template<typename T> T* findInArray(T* arr,uint size,bool max){
    T* value=size>0 ? &arr[0] : nullptr;
    for(int i=0;i<size;i++){
      if(max){
        if(*value<arr[i]){
          value=&arr[i];
        }
      }else{
        if(*value>arr[i]){
          value=&arr[i];
        }
      }
    }
    return value;
  }

  template<typename T> void copyArray(uint array1_size,uint array2_size,T* inputarray1,T* inputarray2){
    if (array1_size < array2_size) {
      array2_size = array1_size;
    }
    for (int i = 0; i < array2_size; i++) {
      inputarray1[i] = inputarray2[i];
    }
  }

  template<typename T> void removeElementFromArray(uint array_size,T* inputarray,uint index){
    if(index<array_size){
      for(int i=0;i<array_size;i++){
        if(i>=index){
          if(i==index){
            array_size--;
          }
          if(i!=array_size){
            inputarray[i]=inputarray[i+1];
          }
        }
      }
    }
  }
};

#endif