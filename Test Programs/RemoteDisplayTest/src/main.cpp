#include <Arduino.h>

#include <vector>

#include "DFRobot_GDL.h"
//#include "button.h"

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

class MainMenu {
   private:
    uint8_t cursorPosition = 0;
    uint8_t cursorMax = 5;

   public:
    void CursorUp();
    void CursorDown();
    void CursorEnter();
};

void MainMenu::CursorUp() {
    if (cursorPosition < cursorMax) {
        screen.fillRoundRect(XMIN + 5, YMIN + 41 + 30 * cursorPosition, XMAX - 10, 27, 10, COLOR_RGB565_BLACK);
        cursorPosition++;
        screen.fillRoundRect(XMIN + 5, YMIN + 41 + 30 * cursorPosition, XMAX - 10, 27, 10, colour1);
    }
}

void MainMenu::CursorDown() {
    if (cursorPosition > 0) {
        screen.fillRoundRect(XMIN + 5, YMIN + 41 + 30 * cursorPosition, XMAX - 10, 27, 10, COLOR_RGB565_BLACK);
        cursorPosition--;
        screen.fillRoundRect(XMIN + 5, YMIN + 41 + 30 * cursorPosition, XMAX - 10, 27, 10, colour1);
    }
}

class Button {
   private:
    const uint8_t pin;
    bool ready = false;
    uint32_t timeOfPress = 0;
    void (*f)();

   public:
    static const uint8_t debounceTime = 150;

    Button(uint8_t pinIn) : pin(pinIn) {}

    void setFunction(void (*func)()) { f = func; }

    const uint8_t getPin() { return pin; }

    bool getReady() { return ready; }

    void pressed() {
        ready = false;
        timeOfPress = millis();
    }

    void setReady(bool ready) { this->ready = ready; }

    uint32_t getTimeOfPress() { return timeOfPress; }

    void callFunction() { f(); }
};

void processButton(Button &btn) {
    if ((digitalRead(btn.getPin()) == LOW) && btn.getReady()) {
        btn.pressed();
        btn.callFunction();
    } else if ((digitalRead(btn.getPin()) == HIGH) &&
               (millis() - btn.getTimeOfPress() > btn.debounceTime)) {
        btn.setReady(true);
    }
}

Button btnUp(D2);
Button btnDown(D3);
Button btnEnter(D5);

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

    // highlighter
    // screen.fillRoundRect(XMIN+5, YMIN+41, XMAX-10, 27,10, colour1);
    screen.fillRoundRect(XMIN + 5, YMIN + 41 + 30, XMAX - 10, 27, 10, colour1);

    MainMenu mainMenu;

    btnUp.setFunction(&mainMenu.CursorUp);
    btnDown.setFunction(&mainMenu.CursorDown);
    btnEnter.setFunction(&mainMenu.CursorEnter);

    screen.setFont(&FreeMono18pt7b);
    screen.setCursor(65, YMIN + 25);
    screen.print("Main Menu");

    String menuItems[] = {"Scan For Devices", "Scan and Retrieve Data",
                          "View Data", "Format SD Card", "About"};

    for (int i = 0; i < 5; i++) {
        screen.setFont(&FreeMono12pt7b);
        screen.setCursor(5, YMIN + 60 + (i * 30));
        screen.print(menuItems[i]);
    }

    screen.fillRect(XMIN, YMAX - 35, XMAX, 35, darkBlue);
    screen.setFont(&FreeMono18pt7b);
    screen.setCursor(5, YMAX - 10);
    screen.print("Other Options");
}

void loop() {
    processButton(btnUp);
    processButton(btnDown);
    processButton(btnEnter);
}