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
    void AddDevice(SavedDevice deviceIn);
    SavedDevice GetDevice(uint8_t index);
    inline uint8_t GetDeviceCount();

};

#endif Saved_H