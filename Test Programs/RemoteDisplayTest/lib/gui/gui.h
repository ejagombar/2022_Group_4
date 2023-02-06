#include <Arduino.h>


class MainMenu {
   private:
    uint8_t cursorPosition = 0;
    static const uint8_t highLightPadding = 5;
    static const uint8_t listLength = 5;
    const String menuItems[listLength] = {"Scan For Devices", "Scan and Retrieve Data", "View Data", "Format SD Card", "About"};

    void SelectItem(uint8_t index);
    void UnselectItem(uint8_t index);

   public:
    void CursorUp();
    void CursorDown();
    void CursorEnter();
    void Initialise();
    void DrawNavbar();
};

class GUI {
   private:
    MainMenu mainMenu;

   public:
    GUI();
    void initilise();
    void btnUpPressed();
    void btnDownPressed();
    void btnEnterPressed();
};
