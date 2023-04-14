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

struct metadata {
    uint8_t ID;
    uint16_t sampleNum;
    uint16_t transmittedNum;
};

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

measurement ArrToStruct(uint8_t* arrIn);
void StructToArr(measurement measurementIn, uint8_t* arrOut);

class SDInterface {
   private:
    uint8_t deviceID;
    OpenFile openFile = NoneOpen;
    File CurrentFile;
    String datalogFilename = "/logdata.bin";
    String errorlogFilename = "/errorlog.txt";
    String metadataFilename = "/metadata.bin";

   public:
    SDInterface(){};
    ~SDInterface(){};
    Error Init();
    Error SetUp(uint8_t idIn);
    metadata getMetadata();
    Error setMetadata(metadata metadataIn);
    Error saveMeasurement(int index, const uint8_t* arrIn);
    Error getMeasurements(int index, uint8_t* arrOut, int sampleCount);
    Error logError(String messageIn);
    void DeleteFiles();
};

#endif