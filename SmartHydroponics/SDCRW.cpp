
//SDCard read and write module (Data storage/Retrieval/Logging) module
// Will also need to do fancy things here to work around memory limitations...

#include "SDCRW.h"
#include <SPI.h>
#include <SD.h>
#include <Arduino.h>
#include "ProgMemStr.h"

const PROGMEM char initSDMes[] = "Init SD Card...";
const PROGMEM char initFailMes[] = "Failed to init SDC!";
const PROGMEM char initSuccessMes[] = "SDC initialized.";

const PROGMEM char FileFailMes[] = "Failed to find or create data file!";
const PROGMEM char FileStoreFailMes[] = "Failed to save data!";
const PROGMEM char FileSuccessMes[] = "Data File loaded.";
const PROGMEM char StoreMes[] = "SDC: Storing: ";

#define DATAFILE "SMPData.txt"

File plantData;

SDCRW::SDCRW() { 
}


void SDCRW::init_SDC() {
  //ProgMemStr().printCharMes(initSDMes);
  if (!SD.begin(10)) {
    Serial.println("NSD");
    while (1); //wait indefinitely
  };

  checkDirExists();//See if SmartPonics directory exists, make it if not
  checkIsCalibrated();//Check to see if we have previous calibration data, load it if so. Must be done before anything else,
  // since we only call open file once on the csv file
  
 // initCSV();
  initDataFile();
  //ProgMemStr().printCharMes(initSuccessMes);
  
}

float SDCRW::getCalib(char cFile[]) {
  plantData = SD.open(cFile, FILE_READ);
 // float cData[];//need to know how many calibration parameters need to be stored
// while (plantData.available()) {
   // float val = 
 //}
 plantData.close();
}

bool SDCRW::checkIsCalibrated() {
  char calFile[] = "SmartHydroponics/calibration.data";
  if (SD.exists(calFile)) {
    return true;
    } else {
      return false;
  };
}

void SDCRW::checkDirExists() {
  String mDir = "SHyd";
  if (not SD.exists(mDir)) {
    SD.mkdir(mDir);
    String heads[5]  = {"F", "8", "24", "W", "30"};
    mkCSVfile(mDir, heads, 5);
  };
}

void SDCRW::mkCSVfile(String subpath, String headers[], int headlen) {
  int file_no;
  //checkDirExists must be called first
  String path;
  while (true) {
    path = subpath + "/pD_" + String(file_no);
    if (SD.exists(path)) {
        file_no++;
      } else {
        break;
      };
  };
  SD.open(path);
  storeCSV(headers, 5, path);
}

void SDCRW::storeCSV(String list[], int listlen, String fname) {
  //plantData = SD.open(fname, FILE_WRITE);
  if (plantData) {
    String str;
    for (int i=0; i<listlen; i++) {
        str += list[i];
        if (i != (listlen - 1)) {
          str += ", ";
          };
        };
    plantData.println(str);
    plantData.close();
  }
};

void SDCRW::initDataFile() {
  plantData = SD.open(DATAFILE, FILE_WRITE);
  if (plantData) {
     ProgMemStr().printCharMes(FileSuccessMes);
  } else {
    ProgMemStr().printCharMes(FileFailMes);
  };

  //plantData.close();
}

bool SDCRW::writeData(String dat, String toFile=DATAFILE) {
  //ProgMemStr().printCharMes(StoreMes, true);
  //Serial.println(dat);
 // plantData = SD.open(toFile, FILE_WRITE);
  if (plantData) {
    //Store data
    plantData.println(dat);
  } else {
    //ProgMemStr().printCharMes(FileStoreFailMes);
    return false;
  };
  //plantData.close();
  delay(25);
  return true;
}
