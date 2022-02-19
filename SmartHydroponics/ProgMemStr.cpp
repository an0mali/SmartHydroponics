#include "ProgMemStr.h"
#include <Arduino.h>

bool enabled = true;

ProgMemStr::ProgMemStr(){
    };
  
void ProgMemStr::printCharMes(const char ptr[],bool endLine=true, const int maxBuffer=64) {
          if (enabled = true) {
            char tbuffer[maxBuffer];
            strcpy_P(tbuffer, ptr);
            String printMes = tbuffer;
            if (endLine) {
              Serial.println(printMes);
            } else {
              Serial.print(printMes);
            };
            delay(50);
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
