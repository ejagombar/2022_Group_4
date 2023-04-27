#include "gui.h"

DFRobot_ST7789_240x320_HW_SPI screen(TFT_DC, TFT_CS, TFT_RST);

void printMenuBar(const String menuItems[3]) {
    const uint8_t NavBarHeight = 25;
    const u_int8_t textHeight = 6;
    // const String menuItems[3] = {"Start", "Stop", "Back"};
    screen.setTextSize(1);
    screen.setFont(&FreeMono12pt7b);
    screen.setTextColor(COLOR_RGB565_LGRAY);

    screen.fillRect(XMIN, YMAX - NavBarHeight, XMAX, NavBarHeight, colour1);

    screen.drawRect((XMAX / 3) - 1, YMAX - NavBarHeight, 2, NavBarHeight, COLOR_RGB565_BLACK);
    screen.drawRect(((2 * XMAX) / 3) - 1, YMAX - NavBarHeight, 2, NavBarHeight, COLOR_RGB565_BLACK);

    // 7 is both the width of a character and the maximum number of characters that can fit in the box
    for (int i = 0; i < 3; i++) {
        int padding = (7 - menuItems[i].length()) * 7;
        screen.setCursor((i * XMAX / 3) + 4 + padding, YMAX - textHeight);
        screen.print(menuItems[i]);
    }
}

//--------------------------------------------------------------------------------------------//

MainMenu::MainMenu() {
    screen.begin();
}

void MainMenu::btnDownPressed() {
    if (cursorPosition < listLength - 1) {
        UnselectItem(cursorPosition);
        cursorPosition++;
        SelectItem(cursorPosition);
    }
}

void MainMenu::btnUpPressed() {
    if (cursorPosition > 0) {
        UnselectItem(cursorPosition);
        cursorPosition--;
        SelectItem(cursorPosition);
    }
}

MainState MainMenu::btnEnterPressed() {
    MainState selection;
    switch (cursorPosition) {
        case 0:
            selection = setUpState;
            break;
        case 1:
            selection = dataFetchState;
            break;
        case 2:
            selection = findDeviceState;
            break;
        case 3:
            selection = resetState;
            break;
        case 4:
            selection = helpState;
            break;
        default:
            selection = mainMenuState;
            break;
    };
    return selection;
}

void MainMenu::UnselectItem(uint8_t index) {
    // Draws a black box over the soon-to-be unselected item and redraws the text on top
    screen.fillRoundRect(XMIN + highLightPadding, YMIN + 41 + 30 * index, XMAX - 2 * highLightPadding, 27, 10, COLOR_RGB565_BLACK);
    screen.setFont(&FreeMono12pt7b);
    screen.setCursor(10, YMIN + 60 + (index * 30));
    screen.print(menuItems[index]);
}

void MainMenu::SelectItem(uint8_t index) {
    // Draws a highlighter around the selected item redraws the text on top
    screen.fillRoundRect(XMIN + highLightPadding, YMIN + 41 + 30 * index, XMAX - 2 * highLightPadding, 27, 10, colour1);
    screen.setFont(&FreeMono12pt7b);
    screen.setCursor(10, YMIN + 60 + (index * 30));
    screen.print(menuItems[index]);
}

void MainMenu::InitScreen() {
    screen.setRotation(3);
    screen.setTextSize(1);
    screen.fillScreen(COLOR_RGB565_BLACK);
    screen.setTextColor(COLOR_RGB565_LGRAY);
    screen.setTextWrap(false);

    screen.setFont(&FreeMono18pt7b);
    screen.setCursor(65, YMIN + 25);
    screen.print("Main Menu");
    //  Prints the list of menu options on the screen
    for (int i = 0; i < listLength; i++) {
        screen.setFont(&FreeMono12pt7b);
        screen.setCursor(10, YMIN + 60 + (i * 30));
        screen.print(menuItems[i]);
    }

    const String menuItems[3] = {"Up", "Down", "Select"};
    printMenuBar(menuItems);

    SelectItem(cursorPosition);
}

//--------------------------------------------------------------------------------------------//

void DeviceSetup::InitScreen() {
    screen.setRotation(3);
    screen.setTextSize(1);
    screen.fillScreen(COLOR_RGB565_BLACK);
    screen.setTextColor(COLOR_RGB565_LGRAY);
    screen.setTextWrap(false);

    screen.setFont(&FreeMono18pt7b);
    screen.setCursor(38, YMIN + 25);
    screen.print("Device Setup");

    const String menuItems[3] = {"Start", "", "Back"};
    printMenuBar(menuItems);
}

void DeviceSetup::btnStartScanPressed() {
    screen.fillRect(40, 60, XMAX - 80, 120, COLOR_RGB565_BLACK);
    const String menuItems[3] = {"", "Cancel", "Back"};
    printMenuBar(menuItems);
}

void DeviceSetup::btnCancelPressed() {
    InitScreen();
}

void DeviceSetup::displayIDNum(uint8_t num) {
    screen.drawRoundRect(45, 65, XMAX - 90, 100, 15, COLOR_RGB565_PURPLE);
    screen.setFont(&FreeMonoBold12pt7b);
    screen.setTextColor(COLOR_RGB565_PURPLE);
    screen.setTextSize(5);
    screen.setCursor(XMIN + 60, YMIN + 146);
    char str[4];
    snprintf(str, sizeof(str), "%03d", num);
    screen.print(str);

    const String menuItems[3] = {"Scan", "", "Back"};
    printMenuBar(menuItems);
}

//--------------------------------------------------------------------------------------------//

void DeviceDataFetch::InitScreen() {
    screen.setRotation(3);
    screen.setTextSize(1);
    screen.fillScreen(COLOR_RGB565_BLACK);
    screen.setTextColor(COLOR_RGB565_LGRAY);
    screen.setTextWrap(true);

    screen.setTextSize(1);
    screen.setFont(&FreeMono18pt7b);
    screen.setCursor(46, YMIN + 25);
    screen.print("Fetch Data");

    const String menuItems[3] = {"Fetch", "", "Back"};
    printMenuBar(menuItems);

    linenum = 0;

    screen.setTextSize(1);
    screen.setFont(&FreeMonoBold12pt7b);
}

void DeviceDataFetch::btnStartBroadcastPressed() {
    const String menuItems[3] = {"", "Cancel", "Back"};
    printMenuBar(menuItems);
    screen.setTextSize(1);
    screen.setFont(&FreeMonoBold12pt7b);
}

void DeviceDataFetch::btnCancelPressed() {
    InitScreen();
}

void DeviceDataFetch::showRecievedData(uint8_t monitorNum, uint32_t samplesCount) {
    if (linenum >= 8) {
        screen.fillRect(XMIN, YMIN + 45, XMAX, YMAX - 70, COLOR_RGB565_BLACK);
        linenum = 0;
    }
    screen.setCursor(2, YMIN + 55 + (linenum * 20));
    char buffer[50];
    sprintf(buffer, "%02d samples from Id:%03d ", samplesCount, monitorNum);
    screen.print(buffer);
    linenum++;
}

void DeviceDataFetch::DispayDataRecieve(uint8_t position, uint8_t monitorNum, uint32_t samplesCount) {
    screen.fillRoundRect(XMIN + 5, YMIN + 50 * position, XMAX - 2 * 5, 40, 10, 0x76d2);  // 0x9ed2 //75f2

    screen.setTextSize(1);
    screen.setFont(&FreeMonoBold12pt7b);
    screen.setTextColor(COLOR_RGB565_BLACK);

    screen.setCursor(10, YMIN + 26 + (position * 50));
    char buffer[15];
    sprintf(buffer, "Monitor %03d", monitorNum);
    screen.print(buffer);

    screen.fillRect(XMAX / 2 + 24, 50 * position, 2, 40, COLOR_RGB565_BLACK);

    screen.setTextColor(COLOR_RGB565_BLACK);
    screen.setTextSize(2);
    // screen.setFont(&FreeMono18pt7b);
    sprintf(buffer, "%04d", samplesCount);
    screen.setCursor(XMAX / 2 + 40, YMIN + 32 + (position * 50));
    screen.print(buffer);
}

//--------------------------------------------------------------------------------------------//

void FindDevice::InitScreen() {
    screen.setRotation(3);
    screen.setTextSize(1);
    screen.fillScreen(COLOR_RGB565_BLACK);
    screen.setTextColor(COLOR_RGB565_LGRAY);
    screen.setTextWrap(true);

    screen.setTextSize(1);
    screen.setFont(&FreeMono18pt7b);
    screen.setCursor(38, YMIN + 25);
    screen.print("Find Monitor");

    const String menuItems[3] = {"Next", "On/Off", "Back"};
    printMenuBar(menuItems);
}

void FindDevice::btnCycleSelection() {
}

void FindDevice::btnToggleBuzzer() {
}

void FindDevice::DrawSelectionPanel(uint8_t num, bool buzzerOn) {
    screen.fillRoundRect(20, 70, XMAX - 40, 100, 15, colour1);
    screen.fillRoundRect(30, 85, 129, 70, 10, COLOR_RGB565_PURPLE);

    screen.setTextColor(COLOR_RGB565_WHITE);
    screen.setTextSize(3);
    screen.setCursor(32, 137);
    screen.setFont(&FreeMonoBold12pt7b);

    char buffer[6];
    sprintf(buffer, "%03d", num);
    screen.print(buffer);

    screen.setTextSize(2);
    screen.setTextColor(COLOR_RGB565_BLACK);

    if (buzzerOn) {
        screen.setCursor(XMAX - 105, 106);
        screen.print("ON");
        screen.fillCircle(XMAX - 75, 135, 22, COLOR_RGB565_DGREEN);
    } else {
        screen.setCursor(XMAX - 120, 106);
        screen.print("OFF");
        screen.fillCircle(XMAX - 80, 135, 22, darkRed);
    }
}

//--------------------------------------------------------------------------------------------//

void HelpPage::InitScreen() {
    screen.setRotation(3);
    screen.setTextSize(1);
    screen.fillScreen(COLOR_RGB565_BLACK);
    screen.setTextColor(COLOR_RGB565_LGRAY);
    screen.setTextWrap(true);

    screen.setFont(&FreeMono18pt7b);
    screen.setCursor(124, YMIN + 25);
    screen.print("Help");

    const String menuItems[3] = {"Prev", "Next", "Back"};

    printMenuBar(menuItems);
    drawPage();
}

void HelpPage::btnNextPressed() {
    if (pageNum < (pageMax - 1)) {
        pageNum++;
        drawPage();
    }
}

void HelpPage::btnPrevPressed() {
    if (pageNum > 0) {
        pageNum--;
        drawPage();
    }
}

void HelpPage::btnBackPressed() {
}

void HelpPage::drawPage() {
    screen.fillRect(XMIN, YMIN + 45, XMAX, 165, COLOR_RGB565_BLACK);
    screen.setCursor(5, YMIN + 60);
    screen.print(contents[pageNum]);
}

//--------------------------------------------------------------------------------------------//

void ErrorPage::InitScreen(String msg) {
    screen.setRotation(3);
    screen.setTextSize(1);
    screen.fillScreen(COLOR_RGB565_BLACK);
    screen.setTextColor(COLOR_RGB565_LGRAY);
    screen.setTextWrap(true);

    screen.setFont(&FreeMono18pt7b);
    screen.setCursor(124, YMIN + 25);
    screen.print("Error");

    const String menuItems[3] = {"", "", "Restart"};

    printMenuBar(menuItems);

    printMenuBar(menuItems);
    screen.setTextSize(1);
    screen.setFont(&FreeMonoBold12pt7b);
    screen.setCursor(2, YMIN + 80);
    screen.print(msg);
}

void ErrorPage::btnNextPressed() {
}

void ErrorPage::btnPrevPressed() {
}

void ErrorPage::btnRestartPressed() {
    ESP.restart();
}
