#ifndef BMPFluidCalc_h
#define BMPFluidCalc_h
#include "DualBMP.h"
#include "PlantData.h"
#include <Arduino.h>

class BMPFluidCalc {
  public:
    BMPFluidCalc();
    void calibrateFluidMeter(DualBMP *dbmp, PlantData *plantdata);
    void senseAndReport(DualBMP *dbmp, bool verbPressure=false);
    void printAvgFluidLvl();
    float currentFluidLevelMean;
    float sensFluidLevel;
    float curTemp;
  private:
    void inputPause();
    void countdownTime(int minutes, int secs);
    void calc_Avgs();
    void calcFluidLevelAvg();
    void calcFluidLevel();
    void reportData(float pressureAmt[2], float temp);
    void calibrateMinLvl(DualBMP *dbmp);
    void calibrateMaxLvl(DualBMP *dbmp);
};

#endif
