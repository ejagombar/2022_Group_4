#include <Arduino.h>
#include <RTClib.h>

#include "button.h"
#include "espnow.h"
#include "gui.h"

//----------------------------------------------------------------------------------------

MainMenu mainMenu;
DeviceSetup deviceSetup;
DeviceDataFetch deviceDataFetch;
FindDevice findDevice;
HelpPage helpPage;
ErrorPage errorPage;
ESPNowInterface espNow;
SDInterface sdInterface;

MainState programState = mainMenuState;
CommsState commsState;
DeviceBuzzer deviceBuzzer;

unsigned long currentMillis;
unsigned long previousMillis = 0;
const long interval = 150;

#define SECONDS_FROM_1970_TO_2023 1672531200
char time_format_buf[] = "DD/MM/YY, hh:mm:00";

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

        case dataFetchState:
            if ((commsState == WaitForUserInput) || (commsState == CommsComplete)) {
                deviceDataFetch.btnStartBroadcastPressed();
                commsState = SendReceive;
                espNow.enableDeviceScanCallback();
            }
            break;

        case findDeviceState:
            findDevice.btnCycleSelection();

            deviceBuzzer.num++;
            if (deviceBuzzer.num >= sdInterface.GetDeviceCount()) {
                deviceBuzzer.num = 0;
            }
            findDevice.DrawSelectionPanel(sdInterface.GetDevice(deviceBuzzer.num).id, deviceBuzzer.buzzerOn);
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

        case dataFetchState:
            if (commsState == SendReceive) {
                deviceDataFetch.btnCancelPressed();
                commsState = WaitForUserInput;
                espNow.disableCallback();
            }
            break;

        case findDeviceState:
            deviceBuzzer.buzzerOn = !deviceBuzzer.buzzerOn;
            findDevice.DrawSelectionPanel(sdInterface.GetDevice(deviceBuzzer.num).id, deviceBuzzer.buzzerOn);
            findDevice.btnToggleBuzzer();
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
            if (programState == dataFetchState) {
                commsState = WaitForUserInput;
                deviceDataFetch.InitScreen();
                // espNow.init();
            }
            if (programState == findDeviceState) {
                findDevice.InitScreen();
                findDevice.DrawSelectionPanel(sdInterface.GetDevice(deviceBuzzer.num).id, deviceBuzzer.buzzerOn);
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
            programState = mainMenuState;
            mainMenu.InitScreen();
            break;

        case dataFetchState:
            espNow.disableCallback();
            programState = mainMenuState;
            mainMenu.InitScreen();
            break;

        case findDeviceState:
            programState = mainMenuState;
            mainMenu.InitScreen();

            break;

        case helpState:
            programState = mainMenuState;
            mainMenu.InitScreen();
            break;
    }
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
    uint8_t count = sdInterface.GetDeviceCount();
    espNow.setDeviceCount(count);
    for (int i = 0; i < count; i++) {
        SavedDevice temp = sdInterface.GetDevice(i);
        espNow.addDevice(temp.macAddr);
        Serial.println("Added Device: ");
        printMAC(temp.macAddr);
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
            deviceSetup.displayIDNum(espNow.getDeviceCount());
            SavedDevice temp = {espNow.getDeviceCount()};
            memcpy(temp.macAddr, espNow.getCurrentMAC(), 6);
            sdInterface.AddDevice(temp);
        }
    }

    if (programState == dataFetchState && commsState == SendReceive) {
        currentMillis = millis();
        if (currentMillis - previousMillis >= interval) {
            previousMillis = currentMillis;
            struct_RequestMessage request = {};
            request.requestData = true;
            request.enableBuzzer = false;
            espNow.broadcastRequest(request);
        }

        if (espNow.getScanningState() == ProcessNewRequest) {
            uint8_t dataFrame[250] = {};
            memcpy(&dataFrame, espNow.getDataFrame(), sizeof(dataFrame));

            espNow.setScanningState(WaitingForMessage);

            uint8_t id = dataFrame[1];
            uint8_t sampleCount = dataFrame[2];
            char fileNameBuf[8] = {0};
            sprintf(fileNameBuf, "/%03d.txt", id);
            deviceDataFetch.showRecievedData(id, sampleCount);

            sdInterface.openMonitorFile(fileNameBuf);

            for (int i = 0; i < sampleCount; i++) {
                char time_format_buf[] = "DD/MM/YY, hh:mm:00";
                char time[20];

                measurement tmp = ArrToStruct(&dataFrame[3 + 13 * i]);
                DateTime timeOut(tmp.time + SECONDS_FROM_1970_TO_2023);
                memcpy(&time, timeOut.toString(time_format_buf), sizeof(time_format_buf));

                sdInterface.printPacket(tmp, time);
            }
            sdInterface.closeFile();
        }
    }
    if (programState == findDeviceState) {
        currentMillis = millis();
        if (currentMillis - previousMillis >= interval) {
            previousMillis = currentMillis;
            struct_RequestMessage request = {};
            request.monitorID = sdInterface.GetDevice(deviceBuzzer.num).id;
            request.requestData = false;
            request.enableBuzzer = deviceBuzzer.buzzerOn;
            request.disableBuzzer = !deviceBuzzer.buzzerOn;
            espNow.broadcastRequest(request);
        }
    }
}
