#ifndef Saved_H
#define Saved_H

#include <Arduino.h>
#include <SD.h>
#include <SPI.h>

struct measurement {
    uint32_t time;
    uint16_t peatHeight;
    uint16_t waterHeight;
    uint16_t boxTemp;
    uint16_t groundTemp;
    uint16_t humidity;
};

struct SavedDevice {
    uint8_t id;
    uint8_t macAddr[6];
};

enum OpenFile {
    DeviceFileOpen,
    DataFileOpen,
    NoneOpen,
};

measurement ArrToStruct(uint8_t* arrIn);
void StructToArr(measurement measurementIn, uint8_t* arrOut);

class SDInterface {
   private:
    uint8_t deviceCount = 0;
    String deviceFileName = "/DeviceFile.bin";

   public:
    SDInterface(){};
    ~SDInterface(){};
    void AddDevice(SavedDevice deviceIn);
    SavedDevice GetDevice(uint8_t index);
    bool Init();
    void DeleteFiles();
    uint8_t GetDeviceCount();
    void openMonitorFile(char* fileName);
    void closeFile();
    void printPacket(measurement measurementIn, char* dateTime);
};

#endif