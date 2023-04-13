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
};

struct struct_message {
    const uint8_t msgType = PairMessage;
    uint8_t id;
    uint8_t height;
    uint8_t temp;
    uint16_t time;
};

enum PairingState {
    WaitingForPairRequest,
    ProcessNewRequest,
    PairConfirmed,
};

struct struct_pairing {  // new structure for pairing
    const uint8_t msgType = PairMessage;
    uint8_t id;
};

uint8_t serverAddress[6] = {0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA};

class ESPNowClient {
   private:
   public:
    ESPNowClient(){};
    Error init();
    void sendPairRequest();
    int processPairingandGetID();
    void enableDeviceSetupCallback();
    void disableCallback();
};

#endif  // ESPNOWCLIENT_H