#include "BMPFluidCalc.h"
#include "ProgMemStr.h"
#include "DualBMP.h"
#include "PlantData.h"
#include <Arduino.h>
//Liquid meter uses the pressure difference between the atmosphere and a water tower to
// [precisely?] measure the amount of liquid remaining, and rate of loss, in a system over time

const int readSamples = 16; //how many pressure samples to average out, > is more accurate but slower to update. 16 is tested works well
const int readLevelSamples = 16;

//int readTotalPhases = 0;
//float readLevelTotal = 0.0;//we

float actualLevel = 1.0;

const int emptyWait = 15;//60;// time in seconds to wait before calulating empty fluid levels
const int fullWait = 15; // {mins, seconds}
// These pressures should be mostly dependent on the container and should only need to be calibrated once, then reloaded.

float emptyPressure;
float fullPressure;//4.2;

//We're going to try to adjust pressure measurements by temp readouts in proportion to the calibraton temp,
// since testing seems to show if directly proportional this could be affecting accuracy
float curTemp;
float calibExtPress;
//float calibTemp;

float currentPressure[readSamples];//last [int readSamples] current pressure readings, * by 100 so as to not have to use float
int pressureIndex = 0;
int levelIndex = 0;
float currentPressureMean;
float currentFluidLevel[readLevelSamples]; //Hopefully just (currentPressureMean / fullPressureMean) * 100
float currentFluidLevelMean = 1.0;//from 0-200, >200 is full, <100 is empty
//float sensFluidLevel = 1.0;


const PROGMEM char calibrateMes[] = "Enable pump, fill to LOW level. Press button to continue.";
const PROGMEM char diffMes[] = "Calc. min pressure";
const PROGMEM char fillMes[] = "Fill to MAX then press button to continue.";
const PROGMEM char maxPressMes[] = "Full pressure set to: ";
const PROGMEM char calCompMes[] = "Calibration Completed.";
const PROGMEM char cPressMeanMes[] = "currentPressureMean:";
const PROGMEM char cFluidLevelMes[] = " AFL: ";
const PROGMEM char newFluidLevelMes[] = "CFL: ";

const int buttonPin = 5;
bool newData = false;

//float atmosAdjust = 1.0;
bool isCalib;

float p0;
float p1;

PlantData *pdata;

BMPFluidCalc::BMPFluidCalc() {
}

void BMPFluidCalc::senseAndReport(DualBMP *dbmp, bool verbPressure = false) {
  //Updates sensors and reports pressure to system fluid level calculator

  dbmp -> updateSensors();
  
  p0 = dbmp -> P[0];//pressure reading from airstone line
  p1 = dbmp -> P[1];//pressure reading from atmos
  curTemp = dbmp -> T[1];
  
  reportData();

  if (verbPressure) {
    String mes = String(currentPressureMean) + ", ";
    pdata -> sendPData(mes, false);
  };
}

void BMPFluidCalc::inputPause() {
  while (true) {
    int buttonState = digitalRead(buttonPin);
    if (buttonState == LOW) {
      newData = false;
      break;
    };
    delay(10);
  };
}

void BMPFluidCalc::countdownTime(int minutes, int secs) {
  secs += minutes * 60;
  pdata -> sendPData("Waiting " + String(secs) + " seconds.");
  for (int i = 0; i < secs; i++) {
    pdata -> sendPData("-", false);
    delay(1000);
  };
}
void BMPFluidCalc::calibrateFluidMeter(DualBMP *dbmp, PlantData *plantdata) {
  //Essentially the init function. Set calibration button pin and begin calibration
  pinMode(buttonPin, INPUT_PULLUP);
  pdata = plantdata;

  calibrateMinLvl(dbmp);
  
  delay(50);
  calibrateMaxLvl(dbmp);
  //calibTemp = curTemp;
  isCalib = true;//
  // add function to detect when liquid has settled and apply a modifier to bring that point to 100% to compensate for final calibration
  // inaccuracies
}




void BMPFluidCalc::calibrateMinLvl(DualBMP *dbmp) {

  //Fluid at mininum level and pump on before init BMP sensors
  String mes = ProgMemStr().getStr(calibrateMes);
  pdata -> sendPData(mes);
  inputPause();
  pdata -> sendPData("!");
  countdownTime(0, emptyWait);
  dbmp -> beginSense();
  //ProgMemStr().printCharMes(diffMes);
  pdata -> sendPData("!");
  mes = ProgMemStr().getStr(diffMes);
  pdata ->sendPData(mes);
  for (int i = 0; i < readSamples; i++) {
    senseAndReport(dbmp, true);
    calibExtPress += (dbmp -> P[1]);
  };
  calibExtPress /= readSamples;
  emptyPressure = currentPressureMean;//
  pdata -> sendPData("!");
  mes = "\nMin P is: " + String(emptyPressure) + "\n";
  pdata -> sendPData(mes);
  delay(100);
};

void BMPFluidCalc::calibrateMaxLvl(DualBMP *dbmp) {
  String mes = ProgMemStr().getStr(fillMes);
  pdata -> sendPData(mes);
  inputPause();
  pdata -> sendPData("!");
  countdownTime(0, fullWait);//wait min, sec for liquid to settle. Good @ 30, set to 10 for debug
  for (int i = 0; i < readSamples; i++) {
    currentPressure[i] = 0.0;//clear out old pressure readings, doesnt matter functionally, just affects display during calibration
  };
  float new_calibExtPress;
  for (int i = 0; i < readSamples; i++) {
    senseAndReport(dbmp, true);
    new_calibExtPress += (dbmp -> P[1]);
  };

  new_calibExtPress /= readSamples;
  calibExtPress = (calibExtPress + new_calibExtPress) / 2.0;
  fullPressure = currentPressureMean;

  // ProgMemStr().printCharMes(maxPressMes, false);
  mes = String(currentPressureMean + emptyPressure) + ", P: " + String(currentPressureMean) + "\n";
  // Serial.print(mes);
  pdata -> sendPData(mes);
  ProgMemStr().printCharMes(calCompMes);

  for (int i = 0; i < readLevelSamples; i++) {
    currentFluidLevel[i] = 1.0;
  };
  delay(500);
};


float maxComp; // U
void BMPFluidCalc::calc_Avgs() {
  float total = 0.0;
  int divisor = 0;
  for (int i = 0; i < readSamples; i++) {
    if (currentPressure[i] != 0) {
      total += currentPressure[i];
      divisor += 1;
    };
  };

  if (divisor > 0 and total != 0.0) {
    float newPressureMean = total / divisor;
    currentPressureMean = newPressureMean;
  };
}

void BMPFluidCalc::calcFluidLevelAvg() {
  //Serial.println("Calculating avg fluid level");
  float total = 0.0;
  int divisor = 0;
  for (int i = 0; i < readLevelSamples; i++) {
    if (currentFluidLevel[i] != 0) {
      total += currentFluidLevel[i];
      divisor += 1;
    };
  };
  if (divisor >= 1 and total != 0.0) {
    currentFluidLevelMean = total / divisor;
  };
}

void BMPFluidCalc::printAvgFluidLvl() {
  ProgMemStr().printCharMes(cFluidLevelMes, false);
  Serial.print(String(currentFluidLevelMean * 100));
  int tInd = pressureIndex;
  tInd -= 1;
  if (tInd < 0) {
    tInd = readSamples - 1;
  };
  
  Serial.print(" Temp:" + String(curTemp) + "C ");
  Serial.print("Trend:" + String(currentPressure[tInd] / fullPressure));
  //Serial.print("p0=" + String(p0) + "p1=" + String(p1) + " Cp1="  + String(calibExtPress) + " p0p1:" + String(p0-p1));
  Serial.print(" p0p1:" + String(p0-p1));
  Serial.println("\tp1c_dif:" + String(p1 - calibExtPress) );
  delay(50);
}

void BMPFluidCalc::calcFluidLevel() {

  if (currentPressureMean != 0 and fullPressure != 0) {

    float newFluidLevel = (currentPressureMean / (fullPressure) ); // * 100.0;
    
    currentFluidLevel[levelIndex] = newFluidLevel;

    //sensFluidLevel = newFluidLevel;
    levelIndex++;
    if (levelIndex >= readLevelSamples) {
      levelIndex = 0;
    };
    if (isCalib) {
      calcFluidLevelAvg();
      printAvgFluidLvl();
    };
  };
  
}

void BMPFluidCalc::reportData() {
  /*DO NOT TOUCH SHIT IS CASH
   * We need to adjust our pressure difference in proportion to the difference of atmospheric pressure change
   */
  
  
  float pressureDiff = p0 - p1;//Airstone line pressure - airpressure

  pressureDiff -= (emptyPressure);// + calibDiff);
  
  currentPressure[pressureIndex] = pressureDiff;
  calc_Avgs();

  pressureIndex++;

  if (pressureIndex >= readSamples) {
    calcFluidLevel();
    pressureIndex = 0;
  };
}
