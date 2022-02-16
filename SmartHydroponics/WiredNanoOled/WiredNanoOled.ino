#include <SoftwareSerial.h>
SoftwareSerial link(2, 3); // Rx, Tx
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// The pins for I2C are defined by the Wire-library. 
// On an arduino UNO:       A4(SDA), A5(SCL)
// On an arduino MEGA 2560: 20(SDA), 21(SCL)
// On an arduino LEONARDO:   2(SDA),  3(SCL), ...
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

char cString[128];
byte chPosX = 0;
byte chPosY = 0;
byte chPos = 0;
byte ch = 0;
char dataStr[6];

bool isCmd;

void setup() {
  Serial.begin(9600);
  //delay(1000);
  Wire.begin();
  link.begin(9600);
  
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  display.clearDisplay();
  display.setTextSize(1);      // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE); // Draw white text
  //display.setCursor(start_x, start_y);     // Start at top-left corner
  display.cp437(true);         // Use full 256 char 'Code Page 437' font

  drawChars("Initializing...\n");
  Serial.println("Listening");
  drawChars("Listening...\n");
  
}


void drawChars(char* strmes) {
  int ypos = display.getCursorY();
  int xpos = display.getCursorX();
  int meslen = strlen(strmes);
  
  xpos += meslen;
  if (xpos >= 127) {
    ypos += 8;
    display.setCursor(0,ypos);
  } else
  if (ypos >= 63) {
    display.clearDisplay();
    display.setCursor(0,0);
  };
  
  const int sizet = meslen;
  for (int i=0; i<sizet; i++) {
    char dat = strmes[i];
    display.write(dat);
  };
  display.display();
}

void parseCmd(String cmd) {
  
  if (cmd == "!") {
    display.setCursor(0, 0);
    display.clearDisplay();
  } else if (cmd == "!reset") {
    display.setCursor(0, 0);
  };
//};
}

String cmdStr = "";
void getLinkData() {
     while(link.available())
   {
    //read incoming char by char:
     ch = link.read();
     cString[chPos] = ch;
     if (chPos == 0) {
      if (ch == '!') {
        isCmd = true;
      };
      if (isCmd) {
        cmdStr += char(ch);
      };
     };
     chPos++;     
   
   if (ch == '\n' or ch =='&') {
     cString[chPos] = 0;
     if (ch == '&') {
     cString[chPos -1] = ' ';
     };
     chPos = 0;
     //Serial.println(cString);
     if (!isCmd) {
      drawChars(cString);
     } else {
      cmdStr.replace("\n", "");
      parseCmd(cmdStr);
      cmdStr = "";
      isCmd = false;
     };
     };
   };
}
void loop() {
 getLinkData();

}
