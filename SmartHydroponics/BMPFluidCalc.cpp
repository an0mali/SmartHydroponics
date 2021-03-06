#include "BMPFluidCalc.h"
#include "ProgMemStr.h"
#include "DualBMP.h"
#include "PlantData.h"
#include <Arduino.h>
/*Liquid meter uses the pressure difference between the atmosphere and a water tower to
 * [precisely?] measure the amount of liquid remaining, and rate of loss, in a system over time.
 * This module should only interact with the sensors and get differential data, processing it into
 * meaningful fluid level information?
*/

const int hyperSamples = 8; //how many pressure samples to average out during calibration
//float hyperSamples[hyperSamples];

const int emptyWait = 5;//60;// time in seconds to wait before calulating empty fluid levels
const int fullWait = 5; // {mins, seconds}
// These pressures should be mostly dependent on the container and should only need to be calibrated once, then reloaded.

int32_t emptyPressure = 0.0;
int32_t fullPressure;//4.2;

float atmosChange = 0.0;
float fluidLevel;
int fluidLevelDiv = 0;
//We're going to try to adjust pressure measurements by temp readouts in proportion to the calibraton temp,
// since testing seems to show if directly proportional this could be affecting accuracy
float curTemp;
float calibTemp;

float P0P1min;
float ePressP1;
float eRawPressP1;
float FullPressP0;

const PROGMEM char calibrateMes[] = "Enable pump, fill to LOW level. Press button to continue.";
const PROGMEM char diffMes[] = "Calc. min pressure";
const PROGMEM char fillMes[] = "Fill to MAX then press button to continue.";
const PROGMEM char maxPressMes[] = "Full pressure set to: ";
const PROGMEM char calCompMes[] = "Calibration Completed.";

const int buttonPin = 5;
bool isCalib;
bool newData;

PlantData *pdata;

BMPFluidCalc::BMPFluidCalc() {
}

float BMPFluidCalc::getDifferential() {
  //Updates sensors and reports pressure to system fluid level calculator
  dbmp.updateSensors();
  float p0p1 = dbmp.rawP[0] - dbmp.rawP[1];//Differential pressure reading
  // float p0p1 = dbmp.P[0];//Differential pressure reading
   float aDiff = dbmp.rawP[1] - eRawPressP1;
  curTemp = dbmp.T[0];
 // Serial.println("P0P1avg: " + String(fluidLevel) + "\tP0P1: " + String(p0p1) + "\tp1Diff: " + String(aDiff) + "\tp0Diff: " + String(pDiff0) + "\t");
fwefwfew
  p0p1 -= emptyPressure;
  //
  return p0p1;
};

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
  printData("Waiting " + String(secs) + " seconds.");
  for (int i = 0; i < secs; i++) {
    printData("-", false);
    delay(1000);
  };
}

void BMPFluidCalc::init_fluidMeter(PlantData *plantdata) {
  
  //Set references, begin calibration if needed
  pdata = plantdata;
  pinMode(buttonPin, INPUT_PULLUP);
  calibrateFluidMeter();
}
  
void BMPFluidCalc::calibrateFluidMeter() {

  calibrateMinLvl();
  delay(50);
  isCalib = true;
  calibrateMaxLvl();
  
}

void BMPFluidCalc::calibrateMinLvl() {
  //Fluid at mininum level and pump on before init BMP sensors
  String mes = ProgMemStr().getStr(calibrateMes);//Sends "Enable pump, fill to LOW..[etc]
  printData(mes);
  inputPause();
  printData("!");//Tells LCD to clear screen
  countdownTime(0, emptyWait);//Wait emptyWait seconds for liquid to settle before beginging measurement
  dbmp.beginSense();//Tells BMP sensors to initialize
  
  printData("!");//clear LCD
  
  mes = ProgMemStr().getStr(diffMes);
  printData(mes);// Displays "Calc min pressure"

  //Get pressure differential readings at minimum fluid level and average them out
  float ePress = getHyperSample(true);
  emptyPressure = ePress;
  ePressP1 = dbmp.P[1];
  eRawPressP1 = dbmp.rawP[1];
  Serial.println("ePressP1: " + String(ePressP1) + " minP: " + String(ePress));
  printData("!");
  mes = "\nMin P is: " + String(emptyPressure) + "\n";
  printData(mes);
  calibTemp = curTemp;

}
  // Also begin averaging the external pressure reading during calibration

float BMPFluidCalc::checkFluidLevel() {
  fluidLevel = getHyperSample();

};

void BMPFluidCalc::calibrateMaxLvl() {
  String mes = ProgMemStr().getStr(fillMes);
  printData(mes);
  inputPause();
  printData("!");
  countdownTime(0, fullWait);//wait min, sec for liquid to settle. Good @ 30, set to 10 for debug
  
  fullPressure = getHyperSample(true);
  printData("!");
  mes = "Max P: " + String(fullPressure) + "\nCalibration Complete.";
  printData(mes);
  delay(1000);
};

float BMPFluidCalc::getFluidLevel(bool resetAverage=false){

  int pDiff0 = dbmp.P[0] - FullPressP0;
  int pDiff1 = dbmp.P[1] - ePressP1;
  float flevel = (fluidLevel) / (fullPressure);
  Serial.print("P0P1avg: " + String(fluidLevel) + "\tP0P1: " + String(dbmp.P[0] - dbmp.P[1]) + "\tp1Diff: " + String(pDiff1) + "\tp0Diff: " + String(pDiff0) + "\t");

  return flevel;
}

float BMPFluidCalc::getHyperSample(bool verb=false, int samples=0) {
  float avgDiff = 0.0;
  if (samples == 0) { 
    samples = hyperSamples;
  };
  for (int i = 0; i < samples; i++) {
    float diff = getDifferential();
    avgDiff += diff;
    if (verb == true) {
      printData(String(avgDiff / (i+1)) + ", ", false);
    };    
  };
  avgDiff /= samples;

  return avgDiff;
}

void BMPFluidCalc::printData(String mes, bool endline=true, bool toserial=false) {
  //output to oled or serial, for debug/calibration. Handled by PlantData module
  pdata ->sendPData(mes, endline, true, toserial);
}
