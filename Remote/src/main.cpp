#include <Arduino.h>

#include "button.h"
#include "espnow.h"
#include "gui.h"

//----------------------------------------------------------------------------------------

MainMenu mainMenu;
DeviceSetup deviceSetup;
DeviceScan deviceBroadcast;
HelpPage helpPage;
ErrorPage errorPage;
ESPNowInterface espNow;
SDInterface sdInterface;

MainState programState = mainMenuState;
CommsState commsState;

unsigned long currentMillis;
unsigned long previousMillis = 0;
const long interval = 500;

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
            if ((commsState == WaitForUserInput) || (commsState == CommsComplete)) {
                deviceSetup.btnStartScanPressed();
                commsState = SendReceive;
                espNow.enableDeviceSetupCallback();
            }
            break;
        case broadcastState:
            if ((commsState == WaitForUserInput) || (commsState == CommsComplete)) {
                deviceBroadcast.btnStartBroadcastPressed();
                commsState = SendReceive;
                espNow.enableDeviceScanCallback();
            }
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
            if (commsState == SendReceive) {
                deviceSetup.btnCancelPressed();
                commsState = WaitForUserInput;
                espNow.disableCallback();
            }
            break;
        case broadcastState:
            if (commsState == SendReceive) {
                deviceBroadcast.btnCancelPressed();
                commsState = WaitForUserInput;
                espNow.disableCallback();
            }
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
                commsState = WaitForUserInput;
                deviceSetup.InitScreen();
                // espNow.init();
            }
            if (programState == broadcastState) {
                commsState = WaitForUserInput;
                deviceBroadcast.InitScreen();
                // espNow.init();
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
            // espNow.deinit();
            programState = mainMenuState;
            mainMenu.InitScreen();

            break;

        case broadcastState:

            espNow.disableCallback();
            // espNow.deinit();
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
    sdInterface.Init();
    delay(500);
    pinMode(btn0.getPin(), INPUT_PULLUP);
    pinMode(btn1.getPin(), INPUT_PULLUP);
    pinMode(btn2.getPin(), INPUT_PULLUP);

    mainMenu.InitScreen();
    programState = mainMenuState;

    unsigned long currentMillis = millis();
    unsigned long previousMillis = 0;

    espNow.init();
    for (int i = 0; i < sdInterface.GetDeviceCount(); i++) {
        SavedDevice temp = sdInterface.GetDevice(i);
        espNow.addDevice(temp.macAddr);
    }
}

void loop() {
    processButton(btn0);
    processButton(btn1);
    processButton(btn2);
    if (programState == setUpState && commsState == SendReceive) {
        if (espNow.ProccessPairingMessage() == Complete) {
            commsState = CommsComplete;
            espNow.disableCallback();
            deviceSetup.displayIDNum(espNow.getMaxId());
            SavedDevice temp = {espNow.getMaxId()};
            memcpy(temp.macAddr, espNow.getCurrentMAC(), 6);
            sdInterface.AddDevice(temp);
        }
    }
    if (programState == broadcastState && commsState == SendReceive) {
        currentMillis = millis();
        if (currentMillis - previousMillis >= interval) {
            previousMillis = currentMillis;
            struct_RequestMessage request = {};
            request.requestData = true;
            request.enableBuzzer = true;
            espNow.broadcastRequest(request);
            Serial.println("Broadcasting Request");
        }

        if (espNow.getScanningState() == RecievedData) {
            uint8_t dataFrame[250];
            memcpy(&dataFrame, espNow.getDataFrame(), sizeof(dataFrame));

            espNow.setScanningState(BroadcastRequest);

            for (int i = 0; i < 250; i++) {
                Serial.print(dataFrame[i], HEX);
                Serial.print(" ");
            }
        }
    }
}