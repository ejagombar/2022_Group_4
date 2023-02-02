#include <Arduino.h>
#include "DFRobot_GDL.h"
#include <vector>


#define TFT_DC D2
#define TFT_CS D6
#define TFT_RST D3

#define XMIN 0
#define XMAX screen.width()
#define YMIN 0
#define YMAX screen.height()

#define darkBlue 0x0190 //214e
#define colour1 0x012a

DFRobot_ST7789_240x320_HW_SPI screen(/*dc=*/TFT_DC, /*cs=*/TFT_CS, /*rst=*/TFT_RST);


void setup()
{
    Serial.begin(115200);
    screen.begin();
    screen.setRotation(3);
    screen.setTextSize(1);
    screen.fillScreen(COLOR_RGB565_BLACK);
    screen.setTextColor(COLOR_RGB565_LGRAY);
    screen.setTextWrap(false);
    
    // highlighter
    //screen.fillRoundRect(XMIN+5, YMIN+41, XMAX-10, 27,10, colour1);
    screen.fillRoundRect(XMIN+5, YMIN+41+30, XMAX-10, 27,10, colour1);
    


    
    screen.setFont(&FreeMono18pt7b);
    screen.setCursor(65,YMIN+25);
    screen.print("Main Menu");

    String menuItems[] = {"Scan For Devices", "Scan and Retrieve Data", "View Data", "Format SD Card", "About"};

    for (int i = 0; i < 5; i++)
    {
        screen.setFont(&FreeMono12pt7b);
        screen.setCursor(5,YMIN+60+(i*30));
        screen.print(menuItems[i]);
    }

    screen.fillRect(XMIN, YMAX-35,XMAX,35, darkBlue);
    screen.setFont(&FreeMono18pt7b);
    screen.setCursor(5,YMAX-10);
    screen.print("Other Options");
    



}

void loop()
{
}