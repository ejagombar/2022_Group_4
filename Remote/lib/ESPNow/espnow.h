#ifndef ESPNow_H
#define ESPNow_H

#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>
#include <esp_wifi.h>

#include "saved.h"

enum PairingState {
    WaitingForPairRequest,
    ProcessNewRequest,
    PairConfirmed, 
};

enum ScanningState {
    BroadcastRequest,
    RecievedData,
    Finished, 
};


enum InitStatus {
    Success = 0,
    WifiModeFail = -1,
    ESPNowFail = -2
};

enum MessageType {
    PairMessage,
    DataMessage,
    RequestMessage,
};

struct struct_message {
    const uint8_t msgType = DataMessage;
    uint8_t id;
    uint16_t time;
    uint8_t height;
    uint8_t temp;
};

struct struct_pairing {  // new structure for pairing
    const uint8_t msgType = PairMessage;
    uint8_t id;
};

struct struct_RequestMessage {
    const uint8_t msgType = RequestMessage;
    bool requestData = false; //
    bool enableBuzzer = false;
    bool disableBuzzer = false;

};

const uint8_t MACAddress[] = {0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA};

// This is an interface class used to invoke various ESPNow functions.
class ESPNowInterface {
   private:
    uint8_t deviceCount = 0;

   public:
    ESPNowInterface(){};
    ~ESPNowInterface(){};
    int init();
    int deinit();
    void enableDeviceSetupCallback();
    void enableDeviceScanCallback();
    void disableCallback();
    PairingState ProccessPairingMessage();
    ScanningState ProccessScanningMessage();
    void broadcastRequest(struct_RequestMessage request);
    friend bool addPeer(const uint8_t *peer_addr);
    uint8_t getMaxId();
    uint8_t* getCurrentMAC();
};

#endif