#ifndef GUI_H
#define GUI_H

#include <Arduino.h>

#include <vector>

#include "DFRobot_GDL.h"

#define TFT_DC D2
#define TFT_CS D6
#define TFT_RST D3

#define XMIN 0
#define YMIN 0
#define XMAX 320
#define YMAX 240

#define darkBlue 0x0190
#define colour1 0x012a
#define darkRed 0xb800

enum MainState {
    mainMenuState,
    setUpState,
    dataFetchState,
    findDeviceState,
    resetState,
    helpState,
    errorState,
};

enum CommsState {
    WaitForUserInput,
    SendReceive,
    CommsComplete,
};

struct DeviceBuzzer {
    uint8_t num = 0;
    bool buzzerOn = false;
};

//--------------------------------------------------------------------------------------------//

class MainMenu {
   private:
    uint8_t cursorPosition = 0;
    static const uint8_t highLightPadding = 5;
    static const uint8_t listLength = 5;
    const String menuItems[listLength] = {"Setup Monitor", "Fetch Data", "Find Monitor", "Reset", "Help"};

    void SelectItem(uint8_t index);
    void UnselectItem(uint8_t index);

   public:
    void btnUpPressed();
    void btnDownPressed();
    MainState btnEnterPressed();
    void InitScreen();
    ~MainMenu(){};
    MainMenu();
};

//--------------------------------------------------------------------------------------------//

class DeviceSetup {
   public:
    void btnStartScanPressed();
    void btnCancelPressed();
    void displayIDNum(uint8_t num);
    void InitScreen();
    ~DeviceSetup(){};
    DeviceSetup(){};
};

//--------------------------------------------------------------------------------------------//

class DeviceDataFetch {
   private:
    uint8_t linenum = 0;

   public:
    void btnCancelPressed();
    void btnStartBroadcastPressed();
    void InitScreen();
    void DispayDataRecieve(uint8_t position, uint8_t monitorNum, uint32_t samplesCount);
    void showRecievedData(uint8_t monitorNum, uint32_t samplesCount);
    DeviceDataFetch(){};
};

//--------------------------------------------------------------------------------------------//

class FindDevice {
   private:
   public:
    FindDevice(){};
    void btnCycleSelection();
    void btnToggleBuzzer();
    void InitScreen();
    void DrawSelectionPanel(uint8_t num, bool buzzerOn);
};

//--------------------------------------------------------------------------------------------//

class HelpPage {
   private:
    static const uint8_t pageMax = 3;
    uint8_t pageNum = 0;
    void drawPage();

    // The Width of the screen is:   <-------------------->
    const String contents[pageMax] = {"Use the \"Setup        Monitors\" function      to configure a new    device. Turn on the   device first and the  remote to recognize it and assign it a value.",
                                      "\"Fetch Data\" will     enable broadcasting   mode. When a monitor  wakes up, it will     connect to the remote  and transmit any new   data.",
                                      "  Group 4 Project by:\n\n       Ed Agombar\n      Alex Morton\n     Marquis Thomas\n      Yu Liang Gan\n     Xiang Yong Gan"};

   public:
    void btnNextPressed();
    void btnPrevPressed();
    void btnBackPressed();
    void InitScreen();
    ~HelpPage(){};
    HelpPage(){};
};

//--------------------------------------------------------------------------------------------//

class ErrorPage {
   public:
    void btnNextPressed();
    void btnPrevPressed();
    void btnRestartPressed();
    void InitScreen(String msg);
    ~ErrorPage(){};
    ErrorPage(){};
};

//--------------------------------------------------------------------------------------------//

#endif