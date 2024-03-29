#ifndef ESPNow_H
#define ESPNow_H

#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>
#include <esp_wifi.h>

#include "saved.h"

enum MessageState {
    WaitingForMessage,
    ProcessNewRequest,
    Complete,
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

struct struct_pairing {  // new structure for pairing
    const uint8_t msgType = PairMessage;
    uint8_t id;
};

struct struct_RequestMessage {
    const uint8_t msgType = RequestMessage;
    uint8_t monitorID = 0;
    bool requestData = false;  //
    bool enableBuzzer = false;
    bool disableBuzzer = false;
};

const uint8_t MACAddress[] = {0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA};

void printMAC(const uint8_t *mac_addr);

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
    MessageState ProccessPairingMessage();
    MessageState getScanningState();
    void setScanningState(MessageState state);
    void addDevice(uint8_t* deviceMAC);
    
    uint8_t* getDataFrame();
    void broadcastRequest(struct_RequestMessage request);
    friend bool addPeer(const uint8_t* peer_addr);
    uint8_t getDeviceCount();
    void setDeviceCount(uint8_t count);
    uint8_t* getCurrentMAC();
};

#endif