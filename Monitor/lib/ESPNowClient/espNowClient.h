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
    StatusMessage,
};

struct struct_message {
    const uint8_t msgType = DataMessage;
    uint8_t id;
    uint8_t height;
    uint8_t temp;
    uint16_t time;
};

struct struct_status {  // new structure for pairing
    const uint8_t msgType = StatusMessage;
    uint8_t id;
    uint8_t sensorStatus;
};

struct struct_pairing {  // new structure for pairing
    const uint8_t msgType = PairMessage;
    uint8_t id;
};

enum PairingState {
    WaitingForPairRequest,
    ProcessNewRequest,
    PairConfirmed,
};





class ESPNowClient {
   private:
   public:
    ESPNowClient(){};
    Error init();
    void sendPairRequest();
    int processPairingandGetID();
    void enableDeviceSetupCallback();
    void disableCallback();
    void sendStatusMessage(Error sensorStatus, uint8_t id);
};

#endif  // ESPNOWCLIENT_H