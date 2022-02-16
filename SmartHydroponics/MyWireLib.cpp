
#include "MyWireLib.h"
#include <Arduino.h>
//------------------------------------------------------------------------------
// WARNING don't change anything unless you verify the change with a scope
//------------------------------------------------------------------------------
// init pins and set bus high

MyWireLib::MyWireLib()
{
   SCLpinI = 2;
   SDApinI = 3;
   Soss= 3;
   ot=900; //delaytime in micros
   DeviceAddress = 238; //for write
   Timeout = 1000;
   bools[8];
}

uint16_t MyWireLib::Get16bitFromRegister(byte Address)
{
  Handshake();
  
  sendbyte(DeviceAddress);//11101110
  
  if(AKS()==false){};
  //{ Serial.println("Error: AKS1");}
  
  sendbyte(Address);//0xAA
  
  if(AKS()==false){};
  //{ Serial.println("Error: AKS2");}
  
  Breakoff();
  Handshake();
  sendbyte((DeviceAddress+1));//11101111
  
  if(AKS()==false){};
  //{ Serial.println("Error: AKS3");}
  
  byte MSB;
  byte LSB;
  uint16_t MSBLSB;
  
  MSB=ReadByte();
  ACM();
  LSB=ReadByte();
  NACKM();

  MSBLSB = (MSB << 8) | LSB;
  return(MSBLSB); 
}

int32_t MyWireLib::Get24bitFromRegister(byte Address)
{
  Handshake();
  
  sendbyte(DeviceAddress);//11101110
  
  if(AKS()==false){};
  //{ Serial.println("Error: AKS Addres Get");}
  
  sendbyte(Address);
  
  if(AKS()==false){};
  //{ Serial.println("Error: AKS Register Get");}
  
  Breakoff();
  Handshake();
  sendbyte((DeviceAddress+1));//11101111
  
  if(AKS()==false){};
  //{ Serial.println("Error: AKS ReadByte Get");}
  

  int32_t MSB;
  int32_t LSB;
  int32_t XLSB;
  int32_t MSBLSBXLSB;
  

  MSB=ReadByte();
  ACM();
  LSB=ReadByte();
  ACM();
  XLSB=ReadByte();
  NACKM();

  MSBLSBXLSB = (((int32_t)MSB << 16) | ((int32_t)LSB << 8) | ((int32_t)XLSB)) >> (8 - Soss);

  return(MSBLSBXLSB); 
}

void MyWireLib::InitWire()
{  
  pinMode(SCLpinI, OUTPUT); 
  pinMode(SDApinI, OUTPUT); 
  delay(100);
}

void MyWireLib::Handshake()
{
   pinMode(SDApinI, OUTPUT); 
  delayMicroseconds(ot);
   digitalWrite(SDApinI, HIGH);
   digitalWrite(SCLpinI, HIGH);
   delayMicroseconds(ot);
   digitalWrite(SDApinI, LOW);
      delayMicroseconds(ot);
   digitalWrite(SCLpinI, LOW);
         delayMicroseconds(ot);
}

void MyWireLib::sendbyte(byte sendbyte)
{
    pinMode(SDApinI, OUTPUT); 
    ConvertByteToBool(sendbyte);
  for(int each=7; each>=0; each--)
  {
   SendBit(bools[each]);
  }
}

void MyWireLib::SendBit(boolean boolbit)
{
  digitalWrite(SCLpinI, LOW);
  digitalWrite(SDApinI, boolbit);
  delayMicroseconds(ot);
  digitalWrite(SCLpinI, HIGH);
  delayMicroseconds(ot);
  digitalWrite(SCLpinI, LOW);
}

byte MyWireLib::ReadByte()
{
  boolean boolR[8];
  pinMode(SDApinI, INPUT); 
  digitalWrite(SCLpinI, LOW);
  for(int each=7; each>=0; each--) //check if this has to be reversed
    {
    delayMicroseconds(ot);
    digitalWrite(SCLpinI, HIGH);
    delayMicroseconds(ot);
    boolR[each]=digitalRead(SDApinI);
    digitalWrite(SCLpinI, LOW);
    }
  return ConvertBoolToByte(boolR);
}

boolean MyWireLib::AKS()
{
  boolean rvalue = LOW;
  digitalWrite(SCLpinI, LOW);
  pinMode(SDApinI, INPUT); 
  delayMicroseconds(ot);
  digitalWrite(SCLpinI, HIGH);

  delayMicroseconds(ot);
  rvalue=!digitalRead(SDApinI);
  if(rvalue==HIGH)
  {
    digitalWrite(SCLpinI, LOW);
    return rvalue;
  }

  //Serial.println("Timeout: ");
  int count =0;
  while(rvalue == LOW & (count)<(Timeout/(ot*2)))
  {
    digitalWrite(SCLpinI, HIGH);
    delayMicroseconds(ot);
    rvalue=!digitalRead(SDApinI);
    if(rvalue==HIGH)
    {
      digitalWrite(SCLpinI, LOW);
      return rvalue;
    }
    count++;
   // Serial.print(count);
   // Serial.print("..");

    digitalWrite(SCLpinI, LOW);
    delayMicroseconds(ot);
  }
  digitalWrite(SCLpinI, LOW);
  return rvalue;
}

void MyWireLib::ACM()
{
  pinMode(SDApinI, OUTPUT); 
  digitalWrite(SCLpinI, LOW);
  digitalWrite(SDApinI, LOW);
  delayMicroseconds(ot);
  digitalWrite(SCLpinI, HIGH);
  delayMicroseconds(ot);
  digitalWrite(SCLpinI, LOW);
}

void MyWireLib::Breakoff()
{
  pinMode(SDApinI, OUTPUT); 
  digitalWrite(SDApinI, HIGH);
  delayMicroseconds(ot);
  digitalWrite(SCLpinI, HIGH);
}

void MyWireLib::NACKM()
{
  pinMode(SDApinI, OUTPUT); 

  digitalWrite(SCLpinI, LOW);
  delayMicroseconds(ot);  
  digitalWrite(SDApinI, HIGH);
  delayMicroseconds(ot);
  digitalWrite(SCLpinI, HIGH);
  delayMicroseconds(ot);    
  digitalWrite(SCLpinI, LOW);
  delayMicroseconds(ot);
  digitalWrite(SDApinI, LOW);
  delayMicroseconds(ot);
  digitalWrite(SCLpinI, HIGH);
  delayMicroseconds(ot);
  digitalWrite(SDApinI, HIGH);
}

void MyWireLib::sendbytes(byte bytes[], int size)
{
  Handshake();
  
  sendbyte(DeviceAddress);//11101110
  
  if(AKS()!=HIGH){};
 // { Serial.println("Error: AKS Address");}
  
  for(int eachb = 0; eachb < size;eachb++)
  {
    sendbyte(bytes[eachb]);

    if(AKS()==LOW){};
   // { Serial.println("Error: AKS Bytes");}

  }

  NACKM();
}

byte MyWireLib::ConvertBoolToByte(boolean boolt[])
{
 return(1*boolt[0]+2*boolt[1]+4*boolt[2]+8*boolt[3]+16*boolt[4]+32*boolt[5]+64*boolt[6]+128*boolt[7]); 
}

void MyWireLib::ConvertByteToBool(byte bytes) //returns bools[]
{
            int val = 0;
            int bitm = 128;
            int c1 = 0;

            for (int each = 7; each >= 0; each--)
            {
                c1 = bytes / bitm;
                bools[each] = 0;
                if (c1 >= 1)
                {
                    val = val | bitm;
                    bytes = bytes - bitm;
                    bools[each] = 1;
                }
                bitm = bitm / 2;
            }
    return; 
}

void MyWireLib::SetSDAPin(int SDAPin)
{
  SDApinI = SDAPin;
}

void MyWireLib::SetSCLPin(int SCLPin)
{
  SCLpinI = SCLPin;
}
