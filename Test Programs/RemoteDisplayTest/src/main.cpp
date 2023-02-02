#include <Arduino.h>

#include <vector>

#include "DFRobot_GDL.h"
#include "button.h"

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

//----------------------------------------------------------------------------------------

#define PADDING 3

class MainMenu {
   private:
    uint8_t cursorPosition = 0;
    static const uint8_t listLength = 5;
    const String menuItems[listLength] = {"Scan For Devices", "Scan and Retrieve Data", "View Data", "Format SD Card", "About"};

    void SelectItem(uint8_t index);
    void UnselectItem(uint8_t index);

   public:
    void CursorUp();
    void CursorDown();
    void CursorEnter();
    void Initialise();
};

void MainMenu::CursorUp() {
    if (cursorPosition < listLength - 1) {
        UnselectItem(cursorPosition);
        cursorPosition++;
        SelectItem(cursorPosition);
    }
}

void MainMenu::CursorDown() {
    if (cursorPosition > 0) {
        UnselectItem(cursorPosition);
        cursorPosition--;
        SelectItem(cursorPosition);
    }
}

void MainMenu::UnselectItem(uint8_t index) {
    // Draws a black box over the soon-to-be unselected item and redraws the text on top
    screen.fillRoundRect(XMIN + PADDING, YMIN + 41 + 30 * index, XMAX - 2 * PADDING, 27, 10, COLOR_RGB565_BLACK);
    screen.setFont(&FreeMono12pt7b);
    screen.setCursor(5, YMIN + 60 + (index * 30));
    screen.print(menuItems[index]);
}

void MainMenu::SelectItem(uint8_t index) {
    // Draws a highlighter around the selected item redraws the text on top
    screen.fillRoundRect(XMIN + PADDING, YMIN + 41 + 30 * index, XMAX - 2 * PADDING, 27, 10, colour1);
    screen.setFont(&FreeMono12pt7b);
    screen.setCursor(5, YMIN + 60 + (index * 30));
    screen.print(menuItems[index]);
    // Draws a bar at the bottom and displays the selected item there
    screen.fillRect(XMIN, YMAX - 35, XMAX, 35, darkBlue);
    screen.setFont(&FreeMono18pt7b);
    screen.setCursor(5, YMAX - 10);
    screen.print(menuItems[index]);
}

void MainMenu::Initialise() {
    screen.setFont(&FreeMono18pt7b);
    screen.setCursor(65, YMIN + 25);
    screen.print("Main Menu");

    //  Prints the list of menu options on the screen
    for (int i = 0; i < listLength; i++) {
        screen.setFont(&FreeMono12pt7b);
        screen.setCursor(5, YMIN + 60 + (i * 30));
        screen.print(menuItems[i]);
    }

    // Selects the first item in the list
    SelectItem(0);
}

//----------------------------------------------------------------------------------------
MainMenu mainMenu;

void processButton(Button &btn) {
    if ((digitalRead(btn.getPin()) == LOW) && btn.getReady()) {
        btn.pressed();
        btn.callFunction();
    } else if ((digitalRead(btn.getPin()) == HIGH) &&
               (millis() - btn.getTimeOfPress() > btn.debounceTime)) {
        btn.setReady(true);
    }
}

void btnUpPressed() {
    mainMenu.CursorUp();
}
void btnDownPressed() {
    mainMenu.CursorDown();
}
void btnEnterPressed() {
}

Button btnUp(D7, &btnUpPressed);
Button btnDown(D9, &btnDownPressed);
Button btnEnter(D5, &btnEnterPressed);

void setup() {
    Serial.begin(115200);

    pinMode(btnUp.getPin(), INPUT_PULLUP);
    pinMode(btnDown.getPin(), INPUT_PULLUP);
    pinMode(btnEnter.getPin(), INPUT_PULLUP);

    screen.begin();
    screen.setRotation(3);
    screen.setTextSize(1);
    screen.fillScreen(COLOR_RGB565_BLACK);
    screen.setTextColor(COLOR_RGB565_LGRAY);
    screen.setTextWrap(false);

    mainMenu.Initialise();
}

void loop() {
    processButton(btnUp);
    processButton(btnDown);
    processButton(btnEnter);
}