/*
    Arduino-Uno Sample Code For ZETA Module useing library.  

   Company:  RF SOLUTIONS   
   web site:  http://www.rfsolutions.co.uk
    

  Sample code Waits till a packet is recived then sends dta out via serial
  19200 baud to read back on a terminal program as #R--0123456789 <crlf>..  

*/

// Include spi Lib
#include <Arduino.h>
#include  <SPI.h>
#include "ZETA.h"

static const uint8_t TX2 = 17;
static const uint8_t RX2 = 16;




static const uint8_t A1 = 39;
static const uint8_t A2 = 34;


static const uint8_t A8 = 2;
static const uint8_t A9 = 13;




static const uint8_t D0 = 3;
static const uint8_t D1 = 1;
static const uint8_t D2 = 25;
static const uint8_t D3 = 26;
static const uint8_t D4 = 27;
static const uint8_t D5 = 0;
static const uint8_t D6 = 14;
static const uint8_t D7 = 13;
static const uint8_t D8 = 5;
static const uint8_t D9 = 2;
static const uint8_t D10 = 17;
static const uint8_t D11 = 16;
static const uint8_t D12 = 4;
static const uint8_t D13 = 12;

/*
#define  SSEL   10   // slave selact pin for SPI
#define  SDN    9    //  sleep pin High = Sleep   low = Active 
#define  IRQ    8    // Interupt request from CODEC
*/

//these pins are configured in the ZETA.Init() function 
#define  TX_LED 7    // ON Board TX LED Controled Via arduino pin 7  
#define  RX_LED LED_BUILTIN    // On Board RX LED Controled Via arduino pin 6

byte RxData;         // rx data byte 
byte PLength;        // Packet lencth read from FIFO


  zeta  ZETA;      // Name the class for use with the library



void setup() {

  ZETA.Init();          // setup ZETA SPI Bus and IO config also setup uart for 19200 baud
  
  ZETA.Zeta_Ready();    // wait for codec to init ready for next command; 
}



void loop() {
  
     ZETA.RX_Mode(0x00, 0X0C);            // configure RX mode on channel 0,  waiting for a 12byte packet
     
     RxData = ZETA.Get_RX_Byte(0X00);      //`#`
       Serial.write(RxData); 

     RxData = ZETA.Get_RX_Byte(0X00);      //`R` 
       Serial.write(RxData);

     RxData = ZETA.Get_RX_Byte(0X00);      // Packet length in HEX
       PLength = RxData;                     // store Packet Length for packet loop
       Serial.write(RxData);

      RxData = ZETA.Get_RX_Byte(0X00);      //`RSSI Value in HEX` 
       Serial.write(RxData);
 
 // Loop till  all data in CODEC FIFO is gone. 
     for(int i = 0; i < (PLength); i ++)
       {
         RxData = ZETA.Get_RX_Byte(0X00);       
         Serial.write(RxData);
       }
 
     delay(20);    // short delay for settle time after getting RX packet
}

