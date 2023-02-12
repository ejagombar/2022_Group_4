#include <Arduino.h>

//--------------------------------------------------------------------------------------------//

// class ListMenuView {
//    private:
//     uint8_t cursorPosition;
//     static const uint8_t highLightPadding;
//     static const uint8_t listLength;
//     const String menuItems[5];

//     virtual void SelectItem(uint8_t index);
//     virtual void UnselectItem(uint8_t index);

//    public:
//     virtual ~ListMenuView(){};
//     virtual void LeftBtn();
//     virtual void MiddleBtn();
//     virtual void RightBtn();
//     virtual void InitScreen() = 0;
// };

//--------------------------------------------------------------------------------------------//

class MainMenu {
   private:
    uint8_t cursorPosition = 0;
    static const uint8_t highLightPadding = 5;
    static const uint8_t listLength = 5;
    const String menuItems[listLength] = {"Scan For Devices", "Scan and Retrieve Data", "View Data", "Format SD Card", "About"};

    void SelectItem(uint8_t index);
    void UnselectItem(uint8_t index);

   public:
    void LeftBtn();
    void MiddleBtn();
    void RightBtn();
    void InitScreen();
    ~MainMenu(){};
    MainMenu(){};
};

//--------------------------------------------------------------------------------------------//

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
