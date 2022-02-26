#ifndef SDCRW_h
#define SDCRW_h
#include <Arduino.h>
#define DATAFILE "SMPData.txt"

class SDCRW {
  public:
    SDCRW();
    void init_SDC();
    bool writeData(String dat, String toFile=DATAFILE);
    void storeHeader(String heads[], int headCnt);
    bool checkIsCalibrated();
    void storeCSV(String list[], int listlen, String fname);
    
    float SDCRW::getCalib(char cFile[]);
    
  private:
    void initDataFile();
    bool checkIsCalib();
    void checkDirExists();
    void SDCRW::mkCSVfile(String subpath, String headers[], int headlen);
    
};

#endif
