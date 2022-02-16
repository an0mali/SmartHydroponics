#ifndef SDCRW_h
#define SDCRW_h
#include <Arduino.h>
#define DATAFILE "SMPData.txt"

class SDCRW {
  public:
    SDCRW();
    void init_SDC();
    bool writeData(String dat, String toFile=DATAFILE);
  private:
    void initDataFile();
};

#endif