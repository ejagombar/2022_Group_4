#ifndef Saved_H
#define Saved_H

#include <Arduino.h>

struct SavedDevice {
    uint8_t id;
    uint16_t macAddr;
};

class Saved {
   private:
    SavedDevice savedDeviceArr[256];
    uint8_t deviceCount = 0;
   public:
    Saved(){};
    ~Saved(){};
    void AddDevice(SavedDevice device);
    SavedDevice GetDevice(uint8_t index);
    uint8_t GetDeviceCount();
};

#endif Saved_H