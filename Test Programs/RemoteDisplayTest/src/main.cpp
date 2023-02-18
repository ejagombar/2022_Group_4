#include <Arduino.h>

#include "button.h"
#include "gui.h"

//----------------------------------------------------------------------------------------

MainMenu mainMenu;
DeviceSetup deviceSetup;
State programState = mainMenuState;


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
    switch (programState) {
        case mainMenuState:
            mainMenu.btnUpPressed();
            break;
        case setUpState:
            deviceSetup.btnUpPressed();
            break;
    }
}
void btn1PressedFunc() {
    switch (programState) {
        case mainMenuState:
            mainMenu.btnDownPressed();
            break;
        case setUpState:
            deviceSetup.btnDownPressed();
            break;
    }
}
void btn2PressedFunc() {
    switch (programState) {
        case mainMenuState:
            if (mainMenu.btnEnterPressed() == setUpState) {
                deviceSetup.InitScreen();
                
            }
            programState = mainMenu.btnEnterPressed();
            break;
        case setUpState:
            break;
    }
    Serial.println(programState);
}

Button btn0(D7, &btn0PressedFunc);
Button btn1(D9, &btn1PressedFunc);
Button btn2(D5, &btn2PressedFunc);

void setup() {
    Serial.begin(115200);

    pinMode(btn0.getPin(), INPUT_PULLUP);
    pinMode(btn1.getPin(), INPUT_PULLUP);
    pinMode(btn2.getPin(), INPUT_PULLUP);

    mainMenu.InitScreen();
    programState = mainMenuState;
}

void loop() {
    processButton(btn0);
    processButton(btn1);
    processButton(btn2);

}