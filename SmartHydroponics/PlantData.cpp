//Record, monitor, and display plant data.
#include "PlantData.h"
#include <Arduino.h>
#include "SDCRW.h"
#include <SoftwareSerial.h>

SoftwareSerial link(2, 3);
SDCRW sdcrw;// SDcard data r/w class



const long day = 86400000; // 86400000 milliseconds in a day
const long hour = 3600000; // 3600000 milliseconds in an hour
const long minute = 60000; // 60000 milliseconds in a minute
const long second =  1000; // 1000 milliseconds in a second

int elapsedHours = 0;
int elapsedDays = 0;
int fifteenMinInt = 0;

float hourFAvgTotal = 0.0; //every 4 hours calculate average level during that period
int hourFAvgDiv = 0;
int hourFCount = 0;//ticks up by 1 each hour, until ==4 at which point we call doHourFour()
float prevFourHourAvg;

//consumeRate is liquid consumption over 24 hours as calculated based on differences between previous two 4-hour periods
//as execute in doHourFour()
int daysUntilEmpty = 9999;//set to 9999 as a marker that it has not yet been calc'd, as <1 will be used to indicate refill is needed now
float consumeRate = 0.0;
float prevConsumeRate = 0.0;

float consumeLevel = 1.0;//Stores hourly avg level data for rate consumption calcuation
float prevConsumeLevel = 1.0;

float FluidLevel = 1.0;//storage for previously passed values, because display and timer intervals may be asyncronous
float Temp;

PlantData::PlantData() {
  
}

void PlantData::init_PlantData() {
 delay(500);
 link.begin(9600);
 link.println("!");
 link.println("Link established.");
 sdcrw.init_SDC();//initialize SD card
 sdcrw.writeData("SDCard initialized");
 //link.println("Calibrating...");
 
}

String PlantData::getRuntime() {

  //Gets current runtime in days, hours, seconds, return as string. Maybe change to char array?
  //This will need to be modified to get around nano 47 day or so limit
  long timeNow = millis();
    
  int days = timeNow / day ;                                //number of days
  int hours = (timeNow % day) / hour;                       //the remainder from days division (in milliseconds) divided by hours, this gives the full hours
  int minutes = ((timeNow % day) % hour) / minute ;         //and so on...
  int seconds = (((timeNow % day) % hour) % minute) / second;

  if (days > elapsedDays) {
    elapsedDays = days;
    doDaily();
  };

  String rTime = String(days) + "d " + String(hours) + "h " + String(minutes) + "m " + String(seconds) + "s";
  //String rTime = "Uptime: " + String(runTime / 1000);
  return rTime;
}

void PlantData::doFifteen() {
  //call this via PlantMP at its 15 minute check interval
  hourFAvgTotal += FluidLevel;
  hourFAvgDiv++;
  fifteenMinInt++;
  if (fifteenMinInt >= 4) {
    fifteenMinInt = 0;
    doHourly();
  };
  recordPlantData();
  
}

void PlantData::doDaily() {
  //probably going to do some sort of daily reporting here
}

void PlantData::doHourly(){
  hourFCount++;
  if (hourFCount == 8) {
    doHourFour();//
  };
}

  
void PlantData::doHourFour() {
  //Calculate consum rates based on four hour averages here, call via doHourly()
  float fourHourAvg = hourFAvgTotal / hourFAvgDiv;

  if (prevFourHourAvg != 0.0) {
    //as percentage of container max volume - container min volume (set during calibration)
    //calc
    float consumeFRate= prevFourHourAvg - fourHourAvg;//loss over 8 hours
    if (consumeFRate > 0) {
      prevConsumeRate = consumeRate;
      consumeRate = consumeFRate * 3.0;//convert to loss over one day, should help amplify changes in uptake that could help ID problems
    //is also useful for more things

      daysUntilEmpty = FluidLevel / consumeRate;
    };
    //
  };
  prevFourHourAvg = fourHourAvg;
  hourFAvgTotal = 0.0;
  hourFAvgDiv = 0;
  hourFCount = 0;
}

void PlantData::updatePlantData(float avgcurrentLevel, float sensLevel, float temp){
  FluidLevel = avgcurrentLevel;
  Temp = temp;
  updateOLED();
}
  
void PlantData::updateOLED() {

  sendPData("!");
  String dat = getStrDat();
  sendPData(dat);
  //dat = "Temp: " + String(temp) + "C";
 // sendPData(dat);
  String daysLeft;
  if (daysUntilEmpty != 9999) {
    if (daysUntilEmpty > 1) {
      daysLeft = String(daysUntilEmpty);
  } else {
    daysLeft = "NOW!";
  }
  dat = "Refill ";
  
  if (daysLeft != "NOW!") {
    //If there are more than one day remaining, format message accordingly
      dat += "in " + daysLeft + " days";  
  } else {
    dat += daysLeft;
  };
  
  sendPData(dat);
};
}

void PlantData::sendPData(String dat, bool endLine=true) {
  //Serial.println("Sending: " + dat);
  if (endLine) {
  link.println(dat);
  } else {
    link.print(dat + "&");
  };
  delay(100);
}

String PlantData::getStrDat() {
  String dat = getRuntime();
  dat += "\n\nFluid Level: " + String(FluidLevel * 100) + "%" + "\nTemp: " + String(Temp) + "C" ;
  String consume = String(consumeRate);
  if (consumeRate == 0) {
    consume = "NotRdy";
  };
  dat += "\nCon: " + consume;
  if (consumeRate != 0 and prevConsumeRate != 0) {
    if (consumeRate > prevConsumeRate) {
      dat += ":)";
    } else {
      dat += ":(";
    };
    };
  
  dat += "\n";
  
  return dat;
}

void PlantData::recordPlantData(){ //, float intervalavg) {

  String dat = getRuntime() + getStrDat();

  sdcrw.writeData(dat);
}
