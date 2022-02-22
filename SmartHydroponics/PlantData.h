#ifndef PlantData_h
#define PlantData_h

#include <Arduino.h>

class PlantData {
  public:
    PlantData();
    void init_PlantData();
    void recordPlantData();//, float intervalavg);
    void init_Display();
    void drawChars(uint16_t start_x, uint16_t start_y, char* strmes);
    String getStrDat();
    String getRuntime();
    
    void sendPData(String dat, bool endLine=true, bool toOLED=true, bool toSerial=false);
    void doFifteen();
    void reportPlantData(float avgcurrentLevel, float temp0, float temp1);
    void updateOLED();    
    
  private:
    void doHourly();
    void doHourFour();
    void doDaily();
    void calcConsumeRate(float currentLevel);

};

#endif
