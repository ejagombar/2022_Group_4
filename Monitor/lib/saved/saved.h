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

void StructToArr(measurement measurementIn, char* arrOut) {
    uint32_t tmp = measurementIn.time << 1;

    memcpy(arrOut, &tmp, 3);
    memcpy(&arrOut[3], &measurementIn.peatHeight, 2);
    memcpy(&arrOut[5], &measurementIn.waterHeight, 2);
    memcpy(&arrOut[7], &measurementIn.boxTemp, 2);
    memcpy(&arrOut[9], &measurementIn.groundTemp, 2);
    memcpy(&arrOut[11], &measurementIn.humidity, 2);
}

measurement ArrToStruct(char* arrIn) {
    measurement measurementOut = {0, 0, 0, 0, 0, 0};
    uint32_t tmp;

    memcpy(&tmp, &arrIn, 3);
    measurementOut.time = tmp >> 1;

    memcpy(&measurementOut.peatHeight, &arrIn[3], 2);
    memcpy(&measurementOut.waterHeight, &arrIn[5], 2);
    memcpy(&measurementOut.boxTemp, &arrIn[7], 2);
    memcpy(&measurementOut.groundTemp, &arrIn[9], 2);
    memcpy(&measurementOut.humidity, &arrIn[11], 2);

    return measurementOut;
}

#endif