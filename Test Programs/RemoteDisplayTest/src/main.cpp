#include <Arduino.h>

#include "button.h"
#include "gui.h"

//----------------------------------------------------------------------------------------
GUI gui;

void processButton(Button &btn) {
    if ((digitalRead(btn.getPin()) == LOW) && btn.getReady()) {
        btn.pressed();
        btn.callFunction();
    } else if ((digitalRead(btn.getPin()) == HIGH) &&
               (millis() - btn.getTimeOfPress() > btn.debounceTime)) {
        btn.setReady(true);
    }
}

void btnUpPressedFunc() {
    gui.btnUpPressed();
}
void btnDownPressedFunc() {
    gui.btnDownPressed();
}
void btnEnterPressedFunc() {
    //gui.btnEnterPressed();
}

Button btnUp(D7, &btnUpPressedFunc);
Button btnDown(D9, &btnDownPressedFunc);
Button btnEnter(D5, &btnEnterPressedFunc);

void setup() {
    Serial.begin(115200);

    pinMode(btnUp.getPin(), INPUT_PULLUP);
    pinMode(btnDown.getPin(), INPUT_PULLUP);
    pinMode(btnEnter.getPin(), INPUT_PULLUP);
}

void loop() {
    processButton(btnUp);
    processButton(btnDown);
    processButton(btnEnter);
}