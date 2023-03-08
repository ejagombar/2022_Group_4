#include <Arduino.h>

#include "button.h"
#include "espnow.h"
#include "gui.h"

//----------------------------------------------------------------------------------------

MainMenu mainMenu;
DeviceSetup deviceSetup;
DeviceScan deviceScan;
HelpPage helpPage;
ErrorPage errorPage;
EPSNowInterface espNow;

MainState programState = mainMenuState;
DeviceSetupState deviceSetupState;

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
            if (deviceSetupState == WaitForUserInput) {
                deviceSetup.btnStartScanPressed();
                deviceSetupState = Scan;
                espNow.init();
                espNow.enableDeviceSetupCallback();
                
            } else if (deviceSetupState == DisplayNumber) {
                deviceSetup.btnStartScanPressed();
            }
            break;
        case scanState:
            deviceScan.btnUpPressed();
            break;
        case helpState:
            helpPage.btnPrevPressed();
            break;
    }
}
void btn1PressedFunc() {
    switch (programState) {
        case mainMenuState:
            mainMenu.btnDownPressed();
            break;
        case setUpState:
            if (deviceSetupState == Scan) {
                deviceSetup.btnCancelPressed();
                deviceSetupState = WaitForUserInput;
                espNow.disableCallback();
            }
            break;
        case scanState:
            deviceScan.btnDownPressed();
            break;
        case helpState:
            helpPage.btnNextPressed();
            break;
    }
}
void btn2PressedFunc() {
    switch (programState) {
        case mainMenuState:

            programState = mainMenu.btnEnterPressed();
            Serial.println(programState);
            if (programState == setUpState) {
                deviceSetupState = WaitForUserInput;
                deviceSetup.InitScreen();
            }
            if (programState == scanState) {
                deviceScan.InitScreen();
            }
            if (programState == helpState) {
                helpPage.InitScreen();
            }
            if (programState == resetState) {
                ESP.restart();
            };

            break;
        case setUpState:

            espNow.disableCallback();
            espNow.deinit();
            programState = mainMenuState;
            mainMenu.InitScreen();

            break;

        case scanState:

            programState = mainMenuState;
            mainMenu.InitScreen();

            break;

        case helpState:

            programState = mainMenuState;
            mainMenu.InitScreen();

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
    if (programState == setUpState && deviceSetupState == Scan) {
        if (espNow.ProccessPairingMessage() == PairConfirmed) {
            deviceSetupState = DisplayNumber;
            deviceSetup.displayIDNum(12);
        }
    }
}