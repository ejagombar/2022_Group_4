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

enum State { MainMenuState, DeviceSetupState, SettingsState };

class MainMenu {
   private:
    uint8_t cursorPosition = 0;
    static const uint8_t highLightPadding = 5;
    static const uint8_t listLength = 5;
    const String menuItems[listLength] = {"Scan For Devices", "Scan and Retrieve Data", "View Data", "Format SD Card", "About"};

    void SelectItem(uint8_t index);
    void UnselectItem(uint8_t index);

   public:
    void btn0PressedFunc();
    void btn1PressedFunc();
    void btn2PressedFunc();
    void InitScreen();
    ~MainMenu(){};
    MainMenu(){};
};

//--------------------------------------------------------------------------------------------//

class Interface {
   private:
    MainMenu mainMenu;

   public:
    Interface();
    void initilise();
    void btn0PressedFunc();
    void btn1PressedFunc();
    void btn2PressedFunc();
};
