#ifndef ProgMemStr_h
#define ProgMemStr_h
#include <Arduino.h>
  /*
  StoreProg is a simple library for retreiving and printing Char arrays from PROGMEM and properly formats and prints them to debugger.

  example- [in .ino]
  
        #include "ProgMemStr.h"
        const PROGMEM char exMes[]  = "String to be stored";
        ProgMemStr sprog;
        setup() {
          sprog.printCharMes(exMes)
          };
        loop(){};

  This can save a HUGE amount of dynamic memory in String-heavy applications.
  
  */ 

class ProgMemStr {
  public:
  
    ProgMemStr();
    void printCharMes(const char ptr[],bool endLine=true, int maxBuffer=64);
    String getStr(const char ptr[], const int maxBuffer=64);
    char getChar(const char ptr[], const int maxBuffer=64);
};

#endif
