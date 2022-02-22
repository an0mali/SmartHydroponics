//Contains SmartMicroponics Functionality, essentially the main thread
#include "SmartMP.h"
#include "PlantData.h"
#include "ProgMemStr.h"
#include "BMPFluidCalc.h"

#include <Arduino.h>

bool isCalibrated;

const unsigned long dataInterval = 900000;//every 15mins secs ms
unsigned long prev_dataTime = 0;
unsigned long runTime;

//const int displayIntervalSecs = 5;//display update rate
const long displayDataInterval = 5 * 1000;
unsigned long prev_displayTime = 0;

BMPFluidCalc bfcalc;
PlantData plantdata;
ProgMemStr *pmstr;

SmartMP::SmartMP() {
}

void SmartMP::SMP_init(){
  //Serial.println("dataInterval is: " + String(dataInterval));
  plantdata.init_PlantData();
  bfcalc.init_fluidMeter(&plantdata);//calibrate fluid level calculations
}

void SmartMP::SMP_loop() {
  bfcalc.checkFluidLevel();
  checkDataUpdate();
}

void SmartMP::checkDataUpdate(){
  runTime = millis();
  unsigned long prevTime = runTime - prev_displayTime;
  if (prevTime > displayDataInterval) {
    float curFluidLevel = bfcalc.getFluidLevel();
    plantdata.updatePlantData(curFluidLevel, bfcalc.dbmp.T[0],bfcalc.dbmp.T[1]);
    prev_displayTime = runTime;
  };
  prevTime = runTime - prev_dataTime;
  
  if (prevTime > dataInterval) {
    //called every 15 mins
    plantdata.doFifteen();
    prev_dataTime = runTime;
  };
}

void SmartMP::calibrateFluidLevel() {
  //Calibrate fluid level
  if (not isCalibrated) {
    plantdata.sendPData("Calibrating...");
    delay(500);
    plantdata.sendPData("!");
    bfcalc.calibrateFluidMeter();
  };
}
