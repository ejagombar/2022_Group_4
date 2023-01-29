// Include spi Lib
#include <Arduino.h>
#include <SPI.h>
#include "ZETA.h"

/*
#define  SSEL   10   // slave selact pin for SPI
#define  SDN    9    //  sleep pin High = Sleep   low = Active
#define  IRQ    8    // Interupt request from CODEC
*/

// these pins are configured in the ZETA.Init() function
#define TX_LED LED_BUILTIN // ON Board TX LED Controled Via arduino pin 7
#define RX_LED LED_BUILTIN // On Board RX LED Controled Via arduino pin 6

unsigned char RxData;  // rx data byte
unsigned char PLength; // Packet lencth read from FIFO

zeta ZETA; // Name the class for use with the library

void setup()
{
  Serial.begin(19200);

  ZETA.Init(); // setup ZETA SPI Bus and IO config also setup uart for 19200 baud

  ZETA.Zeta_Ready(); // wait for codec to init ready for next command;

  ZETA.Set_RF_Baud(0X02); // set RF baud to 9k6

  Serial.write("Listening For Messages...\n");
}

void loop()
{

  ZETA.RX_Mode(0x00, 0X0C); // configure RX mode on channel 0,  waiting for a 12byte packet

  RxData = ZETA.Get_RX_Byte(0X00); //`#`
  Serial.write(RxData);

  RxData = ZETA.Get_RX_Byte(0X00); //`R`
  Serial.write(RxData);

  RxData = ZETA.Get_RX_Byte(0X00); // Packet length in HEX
  PLength = RxData;                // store Packet Length for packet loop
  Serial.write(RxData);

  RxData = ZETA.Get_RX_Byte(0X00); //`RSSI Value in HEX`
  Serial.write(RxData);

  // Loop till  all data in CODEC FIFO is gone.
  for (int i = 0; i < (PLength); i++)
  {
    RxData = ZETA.Get_RX_Byte(0X00);
    Serial.write(RxData);
  }

  delay(20); // short delay for settle time after getting RX packet
}
