// Credits to System via https://forum.arduino.cc/t/how-can-i-add-2-pressure-sensors-bmp180-to-arduino-uno/231589/26
// Code has been modified for a specific project's usecase. Many prints are commented out due to memory consumption
#include "ProgMemStr.h"
#include "DualBMP.h"
#include "MyWireLib.h"

#include <Arduino.h>

const PROGMEM char initMes[]  = "DualBMP: Initializing barometric pressure sensors...";
const PROGMEM char sucessMes[]  = "DualBMP: READY!";

bool showCalibration = false;

//0 == internal/airin/differential sensor, 1== external/rootemp sensor

DualBMP::DualBMP() {
}

void DualBMP::beginSense() {
  ProgMemStr().printCharMes(initMes);//sprog is declared in main .ino

  //declare pins
  Sens[0].SCLpinI = A2;
  Sens[0].SDApinI = A3;
  Sens[1].SCLpinI = A0;
  Sens[1].SDApinI = A1;
  Sens[0].Soss = oss;
  Sens[1].Soss = oss;

  delay(500);
 // Serial.print("SCLPIN for sensor 0: "); Serial.println(Sens[0].SCLpinI);
 // Serial.print("SDAPIN for sensor 0: ");Serial.println(Sens[0].SDApinI);
 // Serial.print("SCLPIN for sensor 1: ");Serial.println(Sens[1].SCLpinI);
 // Serial.print("SDAPIN for sensor 1: ");Serial.println(Sens[1].SDApinI);
  Sens[0].InitWire();
  Sens[1].InitWire();

  init_SENSOR(0);
  init_SENSOR(1);

  ProgMemStr().printCharMes(sucessMes);
}

void DualBMP::updateSensors() {
  int32_t b5[2];
  //Serial.println("Update sensors");
  for (int eachsens = 0; eachsens < 2 ; eachsens++) {
    b5[eachsens] = temperature(eachsens);            // Read and calculate temperature (T)
    P[eachsens] = pressure(b5[eachsens], eachsens);  // Read and calculate pressure (P)
   // printSensorInfo(eachsens);
  };
}

//float DualBMP::getT(int sensor) {
  //Get temperature info from a sensor and return it as a float
 // return T[sensor];
//}

//float DualBMP::getP(int sensor) {
 // return P[sensor];
//}

void DualBMP::printSensorInfo(int eachsens) {
//  Serial.print("Sensor ");
//  Serial.print(eachsens);
//  Serial.print(" => ");
//  Serial.print("Temperature: ");
//  Serial.print(T[eachsens], 2);
//  Serial.print(" C");
//  Serial.print(" | Pressure: ");
//  Serial.print(P[eachsens], 2);
//  Serial.println(" mbar");

}

void DualBMP::init_SENSOR(int sensnr)
{
  ac1[sensnr] = Sens[sensnr].Get16bitFromRegister(0xAA);
  ac2[sensnr] = Sens[sensnr].Get16bitFromRegister(0xAC);
  ac3[sensnr] = Sens[sensnr].Get16bitFromRegister(0xAE);
  ac4[sensnr] = Sens[sensnr].Get16bitFromRegister(0xB0);
  ac5[sensnr] = Sens[sensnr].Get16bitFromRegister(0xB2);
  ac6[sensnr] = Sens[sensnr].Get16bitFromRegister(0xB4);
  b1[sensnr]  = Sens[sensnr].Get16bitFromRegister(0xB6);
  b2[sensnr]  = Sens[sensnr].Get16bitFromRegister(0xB8);
  mb[sensnr]  = Sens[sensnr].Get16bitFromRegister(0xBA);
  mc[sensnr]  = Sens[sensnr].Get16bitFromRegister(0xBC);
  md[sensnr]  = Sens[sensnr].Get16bitFromRegister(0xBE);

  if (showCalibration) {
//  Serial.println("");
//  Serial.print("Sensor ");  Serial.print(sensnr);  Serial.println(" calibration data:");
//  Serial.print(F("AC1 = ")); Serial.println(ac1[sensnr]);
//  Serial.print(F("AC2 = ")); Serial.println(ac2[sensnr]);
//  Serial.print(F("AC3 = ")); Serial.println(ac3[sensnr]);
//  Serial.print(F("AC4 = ")); Serial.println(ac4[sensnr]);
//  Serial.print(F("AC5 = ")); Serial.println(ac5[sensnr]);
//  Serial.print(F("AC6 = ")); Serial.println(ac6[sensnr]);
//  Serial.print(F("B1 = "));  Serial.println(b1[sensnr]);
// Serial.print(F("B2 = "));  Serial.println(b2[sensnr]);
 // Serial.print(F("MB = "));  Serial.println(mb[sensnr]);
//  Serial.print(F("MC = "));  Serial.println(mc[sensnr]);
//  Serial.print(F("MD = "));  Serial.println(md[sensnr]);
//  Serial.println("");
  };
}

/**********************************************
  Calcualte pressure readings
 **********************************************/
float DualBMP::pressure(int32_t b5, int sensnr)
{

  //Maybe experiment with 
  int32_t x1, x2, x3, b3, b6, p, UP;
  uint32_t b4, b7;

  UP = read_pressure(sensnr);  // Read raw pressure
  
  b6 = b5 - 4000;
  x1 = (b2[sensnr] * (b6 * b6 >> 12)) >> 11;
  x2 = ac2[sensnr] * b6 >> 11;
  x3 = x1 + x2;
  b3 = (((ac1[sensnr] * 4 + x3) << oss) + 2) >> 2;
  x1 = ac3[sensnr] * b6 >> 13;
  x2 = (b1[sensnr] * (b6 * b6 >> 12)) >> 16;
  x3 = ((x1 + x2) + 2) >> 2;
  b4 = (ac4[sensnr] * (uint32_t)(x3 + 32768)) >> 15;
  b7 = ((uint32_t)UP - b3) * (50000 >> oss);
  if (b7 < 0x80000000) {
    p = (b7 << 1) / b4;  // or p = b7 < 0x80000000 ? (b7 * 2) / b4 : (b7 / b4) * 2;
  } else {
    p = (b7 / b4) << 1;
  };
  x1 = (p >> 8) * (p >> 8);
  x1 = (x1 * 3038) >> 16;
  x2 = (-7357 * p) >> 16;
  return (p + ((x1 + x2 + 3791) >> 4))/ 100.0f;//will disabling /100 to accuracy since unit of measurement doesnt matter for our purpose?
                                               // ^ this increases sensor volatility? /
}

/**********************************************
  Read uncompensated temperature
 **********************************************/
int32_t DualBMP::temperature(int sensnr)
{
  int32_t x1, x2, b5, UT;
  byte tobesend[2] = {0xf4, 0x2e};
  Sens[sensnr].sendbytes(tobesend, 2);

  delay(5);
  UT = Sens[sensnr].Get16bitFromRegister(0xf6);

  // Calculate true temperature
  x1 = (UT - (int32_t)ac6[sensnr]) * (int32_t)ac5[sensnr] >> 15;
  x2 = ((int32_t)mc[sensnr] << 11) / (x1 + (int32_t)md[sensnr]);
  b5 = x1 + x2;
  T[sensnr]  = (b5 + 8) >> 4;
  T[sensnr] = T[sensnr] / 10.0;                           // Temperature in celsius
  return b5;
}
/**********************************************
  Read uncompensated pressure value
 **********************************************/
int32_t DualBMP::read_pressure(int sensnr)
{
  int32_t value;
  byte tobesend[2] = {0xf4, (0x34 + (oss << 6))};

  Sens[sensnr].sendbytes(tobesend, 2);
  delay(osd);

  value = Sens[sensnr].Get24bitFromRegister(0xf6);
  return (value); // Return value
}
