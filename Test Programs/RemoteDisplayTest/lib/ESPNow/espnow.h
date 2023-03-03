#ifndef ESPNow_H
#define ESPNow_H

#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>
#include <esp_wifi.h>

enum InitStatus {
    Success = 0,
    WifiModeFail = -1,
    ESPNowFail = -2
};

typedef struct struct_message {
    uint16_t time;
    uint8_t id;
    uint8_t height;
    uint8_t temp;
} struct_message;

typedef struct struct_pairing {  // new structure for pairing
    uint8_t msgType;
    uint8_t id;
    uint8_t macAddr[6];
    uint8_t channel;
} struct_pairing;

const uint8_t MACAddress[] = {0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA};

//This is an interface class used to invoke various ESPNow functions.
class EPSNowInterface {
   public:
    EPSNowInterface(){};
    ~EPSNowInterface(){};
    int init();
    int deinit();
    void enableDeviceSetupCallback();
    void enableDeviceScanCallback();
    void disableCallback();
};

#endif