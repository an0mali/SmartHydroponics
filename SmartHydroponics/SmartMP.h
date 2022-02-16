//SmartMicroponics Functionality Header file

#ifndef SmartMP_h
#define SmartMP_h
#include "DualBMP.h"
#include "BMPFluidCalc.h"
#include <Arduino.h>

//needs stuff like display, interaction, and data storage


class SmartMP {
  public:
    SmartMP();
    BMPFluidCalc bfcalc;
    void SMP_init(DualBMP *dbmp);
    void SMP_loop();
    void calibrateFluidLevel(DualBMP *dbmp);
  private:
    void checkDataUpdate();
};

#endif
