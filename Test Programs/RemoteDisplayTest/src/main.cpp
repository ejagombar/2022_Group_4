#include <Arduino.h>

#include "button.h"
#include "gui.h"

//----------------------------------------------------------------------------------------
enum State {
    mainMenuState,
    Start,
    Stop,
    ChangeSpeed
};

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

void btn0PressedFunc() {
    mainMenu.btn0PressedFunc();
}
void btn1PressedFunc() {
    mainMenu.btn1PressedFunc();
}
void btn2PressedFunc() {
    mainMenu.btn2PressedFunc();
}

Button btnUp(D7, &btn0PressedFunc);
Button btnDown(D9, &btn1PressedFunc);
Button btnEnter(D5, &btn2PressedFunc);

void setup() {
    Serial.begin(115200);

    pinMode(btnUp.getPin(), INPUT_PULLUP);
    pinMode(btnDown.getPin(), INPUT_PULLUP);
    pinMode(btnEnter.getPin(), INPUT_PULLUP);

    mainMenu.InitScreen();
}

void loop() {
    processButton(btnUp);
    processButton(btnDown);
    processButton(btnEnter);
}