#include <Arduino.h>

#include <vector>



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
