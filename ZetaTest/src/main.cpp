// Include spi Lib
#include <Arduino.h>
#include  <SPI.h>    // include SPI
#include "ZETA.h"    // RF Solutions Library for ZETA CODEC 


/*
// define controls fro LEDs
#define  SSEL   10   // slave selact pin for SPI
#define  SDN    9    //  sleep pin High = Sleep   low = Active 
#define  IRQ    8    // Interupt request from CODEC
*/

#define  TX_LED 7    // ON Board TX LED Controled Via arduino pin 7  
#define  RX_LED 6    // On Board RX LED Controled Via arduino pin 6

unsigned char RxData;         // rx data byte 
unsigned char PLength;        // Packet lencth read from FIFO

zeta  ZETA;      // Name the class for use with the library



void setup() {

  ZETA.Init();          // setup ZETA SPI Bus and IO config also setup uart for 19200 baud
  
  ZETA.Zeta_Ready();    // wait for codec to init ready for next command; 
  
//  ZETA.Set_RF_Baud(0X02);  // set RF baud to 9k6

}



void loop() {
 
    delay(600);
  
      ZETA.Send_Packet(0x00, 0x0C);        // send a paket on channle 0, 12bytes long   
         ZETA.Write_Byte(0x30);            
         ZETA.Write_Byte(0x31);            // packet received on ZETA-LIB-RX sample = 0, 1, 2, 3, 4, 5, 6, 7 ,8 ,9 <CR LF>
         ZETA.Write_Byte(0x32);            // dispalyed on a terminal program. 
         ZETA.Write_Byte(0x33);
         ZETA.Write_Byte(0x34);
         ZETA.Write_Byte(0x35);
         ZETA.Write_Byte(0x36);
         ZETA.Write_Byte(0x37);
         ZETA.Write_Byte(0x38);
         ZETA.Write_Byte(0x39);
         ZETA.Write_Byte(0x0D);          
         ZETA.Write_Byte(0x0A);
      ZETA.Close_Send();                   // Close send TX data called to finnish data packet. 
}     
