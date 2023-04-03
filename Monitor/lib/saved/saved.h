#ifndef Saved_H
#define Saved_H

#include <Arduino.h>
#include <SD.h>
#include <SPI.h>

struct measurement {
    float time;
    int depth;
    int temperature;
    int humidity;
    int pressure;
};

enum SDError {
    ErrorOpeningFile,
    ErrorWritingFile,
    ErrorClosingFile,
    NoError,
};

enum OpenFile {
    MonitorDataFileOpen,
    DataFileOpen,
    NoneOpen,
};

#define CSPIN D9

class SDInterface {
   private:
    uint8_t deviceID;
    OpenFile openFile = NoneOpen;
   public:
    SDInterface(){};
    ~SDInterface(){};
    SDError setID(uint8_t idIn);
    uint8_t getID();
    SDError saveMeasurement(measurement measurementIn);
    void Init();
    void closeFiles();
};

#endif