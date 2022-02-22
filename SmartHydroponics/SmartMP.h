//SmartMicroponics Functionality Header file

#ifndef SmartMP_h
#define SmartMP_h
#include "BMPFluidCalc.h"
#include <Arduino.h>

//needs stuff like display, interaction, and data storage


class SmartMP {
  public:
    SmartMP();
    void SMP_init();
    void SMP_loop();
  private:
    void checkDataUpdate();
    void calibrateFluidLevel();
};

#endif
