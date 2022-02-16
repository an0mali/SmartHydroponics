
#ifndef MyWireLib_h
#define MyWireLib_h
#include <Arduino.h>

class MyWireLib {
public:
        MyWireLib();
		void InitWire();
		void Handshake();

		void SetSDAPin(int SDAPin);
		void SetSCLPin(int SCLPin);

		void sendbyte(byte sendbyte);
		void sendbytes(byte bytes[], int size);
		void SendBit(boolean boolbit);
		void ConvertByteToBool(byte bytes); //returns bools[]		
		byte ReadByte();
		byte ConvertBoolToByte(boolean boolt[]);
		boolean AKS();
		void ACM();
		void Breakoff();
		void NACKM();
		uint16_t Get16bitFromRegister(byte Address);
		int32_t Get24bitFromRegister(byte Address);
		
		int Soss;
		int Timeout;
		int SCLpinI;
		int  SDApinI;
		int  ot; //delaytime
		int DeviceAddress;

		boolean bools[8];
};
 
#endif
