#ifndef Saved_H
#define Saved_H

#include <Arduino.h>
#include <SD.h>
#include <SPI.h>

struct SavedDevice {
    uint8_t id;
    uint8_t macAddr[6];
};

enum OpenFile {
    DeviceFileOpen,
    DataFileOpen,
    NoneOpen,
};

class SDInterface {
   private:
    uint8_t deviceCount = 0;
    String deviceFileName = "/DeviceFile.bin";

   public:
    SDInterface(){};
    ~SDInterface(){};
    void AddDevice(SavedDevice deviceIn);
    SavedDevice GetDevice(uint8_t index);
    void Init();
    void DeleteFiles();
    uint8_t GetDeviceCount();
};

#endif