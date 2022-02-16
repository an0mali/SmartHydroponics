#include "ProgMemStr.h"
#include <Arduino.h>

ProgMemStr::ProgMemStr(){
    };
  
void ProgMemStr::printCharMes(const char ptr[],bool endLine=true, const int maxBuffer=64) {
          
            char tbuffer[maxBuffer];
            strcpy_P(tbuffer, ptr);
            String printMes = tbuffer;
            if (endLine) {
              Serial.println(printMes);
            } else {
              Serial.println(printMes);
            };
}
char ProgMemStr::getChar(const char ptr[], const int maxBuffer=64) {
            char tbuffer[maxBuffer];
            strcpy_P(tbuffer, ptr);
            return tbuffer;
}
String ProgMemStr::getStr(const char ptr[], const int maxBuffer=64) {
          
            char tbuffer[maxBuffer];
            strcpy_P(tbuffer, ptr);
            String printMes = tbuffer;
            return printMes;
};
