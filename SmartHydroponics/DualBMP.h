//Contains functions for reading from and interpreting BMP180 data from 2 sensors on the same SPI port.
// Credits to System via https://forum.arduino.cc/t/how-can-i-add-2-pressure-sensors-bmp180-to-arduino-uno/231589/26
// Code has been modified for a specific project's usecase (.ino converted to library)

#ifndef DualBMP_h
#define DualBMP_h
#include "MyWireLib.h"
#include <Arduino.h>

class DualBMP {
  public:
    DualBMP();
    void updateSensors();
    void beginSense();
    float T[2], P[2];//Temp, Pressure
    
  private:
    void init_SENSOR(int sensnr);
    int32_t temperature(int sensnr);
    int32_t read_pressure(int sensnr);   
    float pressure(int32_t b5, int sensnr);
    
    const uint8_t oss = 2;                      // Set oversampling setting
    const uint8_t osd = 14;                     // with corresponding oversampling delay
    /*
     * 3 is highest resolution, but for our use case we need to compare 2 sensors as quick as possible and that 
     * high of resolution is unuseable noise.
     * tested: OSS 2 > 3
     */
    
    int16_t  ac1[2], ac2[2], ac3[2], b1[2], b2[2], mb[2], mc[2], md[2]; // Store sensor PROM values from BMP180
    uint16_t ac4[2], ac5[2], ac6[2];                     // Store sensor PROM values from BMP180 
    
    MyWireLib Sens[2];
    
};

#endif
