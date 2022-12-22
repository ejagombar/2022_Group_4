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

#include "Arduino.h"
#include "ZETA.h"

#include "SPI.h"

#define  SSEL   10   // slave select pin for SPI
#define  SDN    9    //  sleep pin High = Sleep   low = Active 
#define  IRQ    8    // Interrupt request from CODEC
#define  TX_LED 7    // ON Board TX LED Controlled Via arduino pin 7  
#define  RX_LED 6    // On Board RX LED Controlled Via arduino pin 6

   unsigned char  SPIVal;  
   unsigned char  Loop_CNT;
	
	
/****************************************************************
	Init:
		configure the SPI Bus And serial port and IO on the 
		Arduino with default Values assigned.    		
		
		setup serial port for 19200 baud rate.
		
****************************************************************/		
void zeta::Init()
{
  pinMode(SSEL, OUTPUT);
  pinMode(TX_LED, OUTPUT);
  pinMode(RX_LED, OUTPUT);
  pinMode(IRQ, INPUT_PULLUP);
  pinMode(12, INPUT_PULLUP);
  pinMode(SDN, OUTPUT);  

  digitalWrite(TX_LED, HIGH);
  digitalWrite(RX_LED, HIGH);
  digitalWrite(SSEL, HIGH);    
  digitalWrite(SDN, LOW);        // hold device in wake state Pull low to sleep
 
  Serial.begin(19200);
 
 // select mode and speed of SPI Bus for  CODEC
  SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));
 // start SPI BUS  
  SPI.begin();	
}



// interrupt from codec to show waiting response from Arduino
void zeta::Wait_IRQ()
{
    while(digitalRead(8) == HIGH){
      // just wait for interrupt from CODEC loop here
    }
}



/****************************************************************
 wait for interrupt from CODEC to show Zeta is ready for commands 
 to be sent. use after Init command 
****************************************************************/
void  zeta::Zeta_Ready()
{
    while (digitalRead(8) == LOW)
	{
		digitalWrite(TX_LED, LOW);		// SHOW WAITING VIA BOTH led LIGHT 
		digitalWrite(RX_LED, LOW);  
    }
	digitalWrite(TX_LED, HIGH);			// turn both LEDs off after ready for use 
	digitalWrite(RX_LED, HIGH);
}



/*************************************************************
 select mode of operation.
      1  =  RX mode
      2  =  READY   
      3  =  Sleep
**************************************************************/ 
void zeta::Select_Mode(unsigned char Mode)
{
    digitalWrite(10, LOW);
    digitalWrite(7,  LOW);
     
      SPI.transfer('A');
      SPI.transfer('T');
      SPI.transfer('M');
      SPI.transfer(Mode); 

    digitalWrite(10, HIGH);
    digitalWrite(7,  HIGH);   
}



/*************************************************************
 
 select RX Mode with custom values (Ch, PLength))
 
       values in HEX 

		Ch 		= Channel number. 
		PLength = pACKET length. 	 
	  
	   after this command wait for IRQ before reading data back 
 
**************************************************************/ 
 void zeta::RX_Mode(unsigned char  Ch, unsigned char PLength) 
 { 
    digitalWrite(SSEL, LOW);
    digitalWrite(RX_LED,  LOW);
     
      SPI.transfer('A');
      SPI.transfer('T');
      SPI.transfer('R');
      SPI.transfer(Ch); 
      SPI.transfer(PLength);  

    digitalWrite(SSEL, HIGH);
    digitalWrite(RX_LED,  HIGH);     
 }
 
 
 /*************************************************************
 
 select RF Baud rate values 0 > 6 See data sheet for settings  
	
       value in HEX 	
	   
	   
 
**************************************************************/ 
 void zeta::Set_RF_Baud(unsigned char Baud)
 { 
	digitalWrite(SSEL, LOW);
	digitalWrite(RX_LED,  LOW);
     
	  SPI.transfer('A');
      SPI.transfer('T');
      SPI.transfer('B');
      SPI.transfer(Baud); 

    digitalWrite(SSEL, HIGH);
    digitalWrite(RX_LED,  HIGH);
	
	digitalWrite(SDN, HIGH);			// device must enter sleep and wake again  
		delay(20);						// with a delay of >= 15ms
	digitalWrite(SDN, LOW);				// wake device and continue use	
 }
 
 
  /*************************************************************
 
 select RF Sync bytes X4 
 
		if not used send 0XAA as the Unused sync bytes

		if only using 3 bytes use 2, 3, 4. 
		if only using 2 bytes use 3, 4.  
		if only using 1 byte use  4.	
		
		unused values must be 0xAA.  
 
       value in HEX 	
 
**************************************************************/ 
 void zeta::Sync_Byte(unsigned char Sync1, unsigned char Sync2, unsigned char Sync3, unsigned char Sync4)
 {
	digitalWrite(SSEL, LOW);
    digitalWrite(RX_LED,  LOW);
     
	  SPI.transfer('A');
      SPI.transfer('T');
      SPI.transfer('A');
 
	  SPI.transfer(Sync1); 
	  SPI.transfer(Sync2);
	  SPI.transfer(Sync3);
	  SPI.transfer(Sync4);
	  
    digitalWrite(SSEL, HIGH);
    digitalWrite(RX_LED,  HIGH);	
 }
 
 
/*************************************************************
 
		Send Packet with (Ch, PLength))
 
       values in HEX 

		Ch 		= Channel number. 
		PLength = pACKET length. 	 
	  
		send your custom packet	
	  
	    after this command pull SSEL line high and TX_LED HIGH.  
 
**************************************************************/ 
 void zeta::Send_Packet(unsigned char Ch, unsigned char PLength)
 {         
    digitalWrite(SSEL, LOW);
    digitalWrite(TX_LED,  LOW);

      SPI.transfer('A');			
      SPI.transfer('T');   
      SPI.transfer('S');
      
      SPI.transfer(Ch);    		 // channel
      SPI.transfer(PLength);     // packet length Over data only  
 }

 
void zeta::Write_Byte(int TxByte)
{
	SPI.transfer(TxByte);	
} 
 
 
 
void  zeta::Close_Send()
{	
	delay(20);
	
	digitalWrite(SSEL, HIGH);
    digitalWrite(TX_LED,  HIGH);
}
 
 
 
 
 
/*************************************************************
Get Packet: 
     read first 3 bytes from  CODEC then loop till packet is complete
	 sends data out to serial at 19200 Dose not return any data.
	 
     SPI BUS PACKET :-  
            BYTE 1   =     '#'
            BYTE 2   =     'R'
            BYTE 3   =     0Xnn      // number of bytes received in packet in HEX value
                         
            BYTE 4   =     0Xnn      // Signal strength of received packet in HEX value         
            BYTE 5 > n =   The Received packet from TX    
**************************************************************/ 
void  zeta::Get_Packet()
{
	digitalWrite(RX_LED, LOW);				
  
    digitalWrite(SSEL, LOW);               // first Byte from FIFO = #
       SPIVal = (SPI.transfer(0x00));
       Serial.write(SPIVal);
    digitalWrite(SSEL, HIGH);              // toggle SSEL every byte 
     
     Wait_IRQ();
     
    digitalWrite(SSEL, LOW);               // Second Byte From FIFO = R    
       SPIVal = (SPI.transfer(0x00));
       Serial.write(SPIVal);
    digitalWrite(SSEL, HIGH);            
        
    Wait_IRQ();
     
    digitalWrite(SSEL, LOW);               // Third Byte = packet Length
       Loop_CNT = (SPI.transfer(0x00));    // Store packet length for FOR LOOP
       Serial.write(Loop_CNT);
    digitalWrite(SSEL, HIGH);            
    
 // loop here till the Total Packet has been Read by SPI and sent out Via Serial 
 // add 1 to the loop as there is stuill the RSSI value to read from FIFO 
   for (int i = 0; i < (Loop_CNT + 1); i++ ){            
      Wait_IRQ();
        digitalWrite(SSEL, LOW);    
           SPIVal = (SPI.transfer(0x00));
           Serial.write(SPIVal);
		digitalWrite(SSEL, HIGH);            //toggle SSEL every byte 
     } 
     
     digitalWrite(RX_LED,  HIGH);           // Default LED to OFF     
}
 
 
 /*************************************************************
Get_RX_Byte:
		returns 1 Byte From the RX FIFO 
		
	example format:	RxData = zeta.Get_RX_Byte(0x00);
							
**************************************************************/ 
int zeta::Get_RX_Byte(int RxByte){
	
	Wait_IRQ();
		
    digitalWrite(RX_LED, HIGH);
	
		digitalWrite(SSEL, LOW);             // Third Byte = packet Length
			RxByte = (SPI.transfer(0x00));                  // Store packet length for FOR LOOP
		digitalWrite(SSEL, HIGH);
	
	digitalWrite(RX_LED, LOW);
	
	return RxByte;	
	
	}