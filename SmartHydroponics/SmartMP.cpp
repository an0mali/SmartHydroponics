//Contains SmartMicroponics Functionality, essentially the main thread
#include "SmartMP.h"
#include "DualBMP.h"
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
DualBMP *d_bmp;
PlantData plantdata;


SmartMP::SmartMP() {
}

void SmartMP::SMP_init(DualBMP *dbmp){
  //Serial.println("dataInterval is: " + String(dataInterval));
  d_bmp = dbmp;
  plantdata.init_PlantData();
  calibrateFluidLevel(d_bmp);//calibrate fluid level calculations
}

void SmartMP::SMP_loop() {
  bfcalc.senseAndReport(d_bmp);
  checkDataUpdate();
}

void SmartMP::checkDataUpdate(){
  runTime = millis();
  unsigned long prevTime = runTime - prev_displayTime;
  if (prevTime > displayDataInterval) {
    plantdata.updateOLED(bfcalc.currentFluidLevelMean, bfcalc.sensFluidLevel, bfcalc.curTemp);
    prev_displayTime = runTime;
  };
  prevTime = runTime - prev_dataTime;
  
  if (prevTime > dataInterval) {
    //called every 15 mins
    plantdata.doFifteen();
    prev_dataTime = runTime;
  };
}

void SmartMP::calibrateFluidLevel(DualBMP *dbmp) {
  //Calibrate fluid level
  if (not isCalibrated) {
    plantdata.sendPData("Calibrating...");
    delay(500);
    plantdata.sendPData("!");
    bfcalc.calibrateFluidMeter(dbmp, &plantdata);
  };
}
