#include "saved.h"

Error SDInterface::Init() {
    if (!SD.begin(CSPIN)) {
        return -1;
    } else {
        return 0;
    }
}

Error SDInterface::saveMeasurement(int index, const uint8_t* arrIn) {
    CurrentFile = SD.open(datalogFilename, FILE_WRITE);
    if (CurrentFile) {
        CurrentFile.seek(index * 13);
        CurrentFile.write(arrIn, 13);

        CurrentFile.close();
        return NO_ERROR;
    } else {
        return FATAL_ERROR;
    }
}

Error SDInterface::getMeasurements(int index, uint8_t* arrOut, int sampleCount) {
    CurrentFile = SD.open(datalogFilename, FILE_READ);
    if (CurrentFile) {
        Serial.print("Starting read from: ");
        Serial.println(index * 13);
        CurrentFile.seek(index * 13);
        CurrentFile.read(arrOut, 13 * sampleCount);
        CurrentFile.close();
        return NO_ERROR;
    } else {
        return FATAL_ERROR;
    }
}

Error SDInterface::logError(String messageIn) {
    CurrentFile = SD.open(errorlogFilename, FILE_APPEND);
    if (CurrentFile) {
        CurrentFile.println(messageIn);
        CurrentFile.close();
        return NO_ERROR;
    } else {
        return FATAL_ERROR;
    }
}

void SDInterface::DeleteFiles() {
    SD.remove(datalogFilename);
    SD.remove(errorlogFilename);
}

void StructToArr(measurement measurementIn, uint8_t* arrOut) {
    uint32_t tmp = measurementIn.time << 0;

    memcpy(&arrOut[0], &tmp, 3);
    memcpy(&arrOut[3], &measurementIn.peatHeight, 2);
    memcpy(&arrOut[5], &measurementIn.waterHeight, 2);
    memcpy(&arrOut[7], &measurementIn.boxTemp, 2);
    memcpy(&arrOut[9], &measurementIn.groundTemp, 2);
    memcpy(&arrOut[11], &measurementIn.humidity, 2);
}

measurement ArrToStruct(uint8_t* arrIn) {
    measurement measurementOut = {0, 0, 0, 0, 0, 0};
    uint32_t tmp = 0;

    memcpy(&tmp, &arrIn[0], 3);

    measurementOut.time = tmp >> 0;

    memcpy(&measurementOut.peatHeight, &arrIn[3], 2);
    memcpy(&measurementOut.waterHeight, &arrIn[5], 2);
    memcpy(&measurementOut.boxTemp, &arrIn[7], 2);
    memcpy(&measurementOut.groundTemp, &arrIn[9], 2);
    memcpy(&measurementOut.humidity, &arrIn[11], 2);

    return measurementOut;
}
