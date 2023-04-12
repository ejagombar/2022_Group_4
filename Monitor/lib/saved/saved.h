#ifndef Saved_H
#define Saved_H

#include <Arduino.h>
#include <SD.h>
#include <SPI.h>

#include "Error.h"

struct measurement {
    uint32_t time;
    uint16_t peatHeight;
    uint16_t waterHeight;
    uint16_t boxTemp;
    uint16_t groundTemp;
    uint16_t humidity;
};

// measurement ArrToStruct(uint8_t* arrIn);
// void StructToArr(measurement measurementIn, uint8_t* arrOut);

enum OpenFile {
    MonitorDataFileOpen,
    DataFileOpen,
    NoneOpen,
};



struct Datastore {
    uint16_t x;
    uint16_t y;
};

#define CSPIN D9

void readIt();
void makeData();

class SDInterface {
   private:
    uint8_t deviceID;
    OpenFile openFile = NoneOpen;
    File CurrentFile;
    String filename = "/logdata2.bin";

   public:
    SDInterface(){};
    ~SDInterface(){};
    Error Init();
    void openFileWrite(OpenFile choice);
    void closeFile();
    Error setID(uint8_t idIn);
    uint8_t getID();
    Error saveMeasurement(const uint8_t* arrIn);
    Error getMeasurements(int index, uint8_t* arrOut,int sampleCount);
    void clearFile();
};


#endif