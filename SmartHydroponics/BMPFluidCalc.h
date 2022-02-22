#ifndef BMPFluidCalc_h
#define BMPFluidCalc_h
#include "DualBMP.h"
#include "PlantData.h"
#include <Arduino.h>

class BMPFluidCalc {
  public:
    BMPFluidCalc();
    void calibrateFluidMeter();
    float checkFluidLevel();
    void init_fluidMeter(PlantData *plantdata);
    float getFluidLevel(bool resetAverage=false);
    DualBMP dbmp;
  private:
    void inputPause();
    void countdownTime(int minutes, int secs);
    float getDifferential();
    float getHyperSample(bool verb=false, int samples=0);
    void calibrateMinLvl();
    void calibrateMaxLvl();
    void printData(String mes, bool endline=true, bool toserial=false);
};

#endif
