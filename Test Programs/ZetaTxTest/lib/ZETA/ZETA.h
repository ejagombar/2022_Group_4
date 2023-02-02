/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	Library for ZETA CODEC Dev board 
	
	Order From: http://www.rfsolutions.co.uk  
		
	freq bands available: 433, 868, 915 	

RF SOLUTIONS.

	COPYRIGHT:		William Alexander House
					William Way, Burgess Hill, West Sussex
					RH15 9AG
					+44 (0)1444 227900
					(01444) 227900 
	
	written by: AB 
	Date:		04/08/2015

		LIB Version:  1.0.0
		
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/


#ifndef ZETA_h
#define ZETA_h

class zeta {
	public:
		void Init();								// configure device	
		void Wait_IRQ();							// Wait for Interrupt for CODEC (DATA RAEDY)  
		void Zeta_Ready();							// wait for device to be ready for next command 
		
		void Select_Mode(unsigned char Mode);				// Device sleep, Ready, RX mode(see data sheet for values) 
		void RX_Mode(unsigned char Ch, unsigned char PLength);		// enter RX mode select channel and packet length 
		void Set_RF_Baud(unsigned char Baud);				// see data sheet for values 
		void Sync_Byte(unsigned char Sync1, unsigned char Sync2, unsigned char Sync3, unsigned char Sync4); // change sync bytes on rf zeta 
		
		void Send_Packet(unsigned char Ch, unsigned char PLength);	// send packet opening 
		void Close_Send();							// close send packet	
		void Write_Byte(int TxByte);				// Write byte to send TX
		void Get_Packet();							// read a packet via spi and send out via serial 
		
		int  Get_RX_Byte(int RxByte);				// read one byte from fifo loop till empty
		// use when needing to presses data from TX packet
};
#endif