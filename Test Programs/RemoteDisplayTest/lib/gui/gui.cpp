#include "gui.h"

#include <Arduino.h>

#include <vector>

#include "DFRobot_GDL.h"

#define TFT_DC D2
#define TFT_CS D6
#define TFT_RST D3

#define XMIN 0
#define YMIN 0
#define XMAX screen.width()
#define YMAX screen.height()

#define darkBlue 0x0190
#define colour1 0x012a

DFRobot_ST7789_240x320_HW_SPI screen(TFT_DC, TFT_CS, TFT_RST);

GUI::GUI() {
    screen.begin();
    screen.setRotation(3);
    screen.setTextSize(1);
    screen.fillScreen(COLOR_RGB565_BLACK);
    screen.setTextColor(COLOR_RGB565_LGRAY);
    screen.setTextWrap(false);
    mainMenu.InitScreen();
}

void GUI::btnUpPressed() {
    mainMenu.LeftBtn();
}

void GUI::btnDownPressed() {
    mainMenu.MiddleBtn();
}

void GUI::btnEnterPressed() {
    mainMenu.RightBtn();
}

//--------------------------------------------------------------------------------------------//

void MainMenu::MiddleBtn() {
    if (cursorPosition < listLength - 1) {
        UnselectItem(cursorPosition);
        cursorPosition++;
        SelectItem(cursorPosition);
    }
}

void MainMenu::LeftBtn() {
    if (cursorPosition > 0) {
        UnselectItem(cursorPosition);
        cursorPosition--;
        SelectItem(cursorPosition);
    }
}

void MainMenu::RightBtn() {
}

void MainMenu::UnselectItem(uint8_t index) {
    // Draws a black box over the soon-to-be unselected item and redraws the text on top
    screen.fillRoundRect(XMIN + highLightPadding, YMIN + 41 + 30 * index, XMAX - 2 * highLightPadding, 27, 10, COLOR_RGB565_BLACK);
    screen.setFont(&FreeMono12pt7b);
    screen.setCursor(5, YMIN + 60 + (index * 30));
    screen.print(menuItems[index]);
}

void MainMenu::SelectItem(uint8_t index) {
    // Draws a highlighter around the selected item redraws the text on top
    screen.fillRoundRect(XMIN + highLightPadding, YMIN + 41 + 30 * index, XMAX - 2 * highLightPadding, 27, 10, colour1);
    screen.setFont(&FreeMono12pt7b);
    screen.setCursor(5, YMIN + 60 + (index * 30));
    screen.print(menuItems[index]);
}

void MainMenu::InitScreen() {
    screen.setFont(&FreeMono18pt7b);
    screen.setCursor(65, YMIN + 25);
    screen.print("Main Menu");
    //  Prints the list of menu options on the screen
    for (int i = 0; i < listLength; i++) {
        screen.setFont(&FreeMono12pt7b);
        screen.setCursor(5, YMIN + 60 + (i * 30));
        screen.print(menuItems[i]);
    }

    const uint8_t NavBarHeight = 25;
    const u_int8_t textHeight = 6;
    const String menuItems[3] = {"Up", "Down", "Select"};

    screen.setFont(&FreeMono12pt7b);

    screen.fillRect(XMIN, YMAX - NavBarHeight, XMAX, NavBarHeight, colour1);

    screen.drawRect((XMAX / 3) - 1, YMAX - NavBarHeight, 2, NavBarHeight, COLOR_RGB565_BLACK);
    screen.drawRect(((2 * XMAX) / 3) - 1, YMAX - NavBarHeight, 2, NavBarHeight, COLOR_RGB565_BLACK);

    // 7 is both the width of a character and the maximum number of characters that can fit in the box
    for (int i = 0; i < 3; i++) {
        int padding = (7 - menuItems[i].length()) * 7;
        screen.setCursor((i * XMAX / 3) + 4 + padding, YMAX - textHeight);
        screen.print(menuItems[i]);
    }

    SelectItem(0);
}

//--------------------------------------------------------------------------------------------//
