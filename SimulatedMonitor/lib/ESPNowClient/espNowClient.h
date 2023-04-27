#ifndef ESPNOWCLIENT_H
#define ESPNOWCLIENT_H

#include <Arduino.h>
#include <EEPROM.h>
#include <WiFi.h>
#include <esp_now.h>
#include <esp_wifi.h>

#include "Error.h"

enum MessageType {
    PairMessage,
    DataMessage,
    RequestMessage,
};

struct struct_pairing {  // new structure for pairing
    const uint8_t msgType = PairMessage;
    uint8_t id;
};

struct struct_RequestMessage {
    const uint8_t msgType = RequestMessage;
    uint8_t monitorID = 0;
    bool requestData = false;
    bool enableBuzzer = false;
    bool disableBuzzer = false;
};

enum MessageState {
    WaitingForMessage,
    ProcessNewRequest,
    Complete,
};

class ESPNowClient {
   private:
   public:
    ESPNowClient(){};

    Error init();
    void enableDeviceSetupCallback();
    void enableRemoteBroadcastListener();
    void disableCallback();

    void sendPairRequest();
    void sendDataPacket(uint8_t *packet);
    void sendPairConfirmation(uint8_t id);

    int processPairingandGetID();
    struct_RequestMessage processRemoteBroadcast();
};

#endif  // ESPNOWCLIENT_H