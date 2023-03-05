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

struct struct_message {
    uint8_t id;
    uint16_t time;
    uint8_t height;
    uint8_t temp;
};

struct struct_pairing {  // new structure for pairing
    const uint8_t id = 0;
    uint8_t newId;
};

const uint8_t MACAddress[] = {0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA};

// This is an interface class used to invoke various ESPNow functions.
class EPSNowInterface {
    friend bool addPeer(const uint8_t *peer_addr);

   private:
    uint8_t deviceCount = 0;

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