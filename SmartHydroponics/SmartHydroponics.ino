//SmartMicroponics system by Dennis Quigley
//This file is essentially just a header for arduino, with everything really starting in SmartMP.h
//Initialize 
//TODO: Move everything else to .h

#include "ProgMemStr.h"
#include "SmartMP.h"
const PROGMEM char initSMMes[] = "\nInitializing SmartMicroponics System...";

SmartMP smartmp;

void setup() {
  Serial.begin(9600);
  while (!Serial) {}; //wait for serial connection to establish
  ProgMemStr().printCharMes(initSMMes);//TODO: Create an object of ProgMemStr and pass it to the other modules
  smartmp.SMP_init();//initilize Smart Microponics Data and Interface systems, pass DualBMP object
  
}
void loop() {
  smartmp.SMP_loop();
}
