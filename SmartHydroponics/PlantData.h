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
    
    void sendPData(String dat, bool endLine=true);
    void doFifteen();
    void updatePlantData(float avgcurrentLevel, float sensLevel, float temp);
    
  private:
    void doHourly();
    void doHourFour();
    void doDaily();
    void calcConsumeRate(float currentLevel);
    void updateOLED();
};

#endif
