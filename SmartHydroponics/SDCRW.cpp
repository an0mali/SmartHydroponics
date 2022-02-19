
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
  ProgMemStr().printCharMes(initSDMes);
  if (!SD.begin(10)) {
    //ProgMemStr().getStr(initFailMes);
    while (1); //wait indefinitely
  };
  initDataFile();
  ProgMemStr().printCharMes(initSuccessMes);
}

void SDCRW::initDataFile() {
  plantData = SD.open(DATAFILE, FILE_WRITE);
  if (plantData) {
     ProgMemStr().printCharMes(FileSuccessMes);
  } else {
    ProgMemStr().printCharMes(FileFailMes);
  };

  plantData.close();
}

bool SDCRW::writeData(String dat, String toFile=DATAFILE) {
  ProgMemStr().printCharMes(StoreMes, true);
  //Serial.println(dat);
  dat = "***\n" + dat;
  plantData = SD.open(toFile, FILE_WRITE);
  if (plantData) {
    //Store data
    plantData.println(dat);
  } else {
    ProgMemStr().printCharMes(FileStoreFailMes);
    return false;
  };
  plantData.close();
  delay(250);
  return true;
}
