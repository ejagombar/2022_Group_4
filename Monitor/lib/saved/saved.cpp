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
    SD.remove(metadataFilename);
}

Error SDInterface::SetUp(uint8_t idIn) {
    metadata metadataIn = {idIn, 0, 0, 0};
    CurrentFile = SD.open(metadataFilename, FILE_WRITE);
    if (CurrentFile) {
        CurrentFile.seek(0);
        CurrentFile.write((byte*)&metadataIn, sizeof(metadataIn));
        CurrentFile.close();
        return NO_ERROR;
    } else {
        return FATAL_ERROR;
    }
}

Error SDInterface::setMetadata(metadata metadataIn) {
    CurrentFile = SD.open(metadataFilename, FILE_WRITE);
    if (CurrentFile) {
        CurrentFile.seek(0);
        CurrentFile.write((byte*)&metadataIn, sizeof(metadataIn));
        CurrentFile.close();
        return NO_ERROR;
    } else {
        return FATAL_ERROR;
    }
}

metadata SDInterface::getMetadata() {
    metadata metadataOut = {};
    CurrentFile = SD.open(metadataFilename, FILE_READ);
    if (CurrentFile) {
        CurrentFile.seek(0);
        CurrentFile.read((byte*)&metadataOut, sizeof(metadataOut));
        CurrentFile.close();
    }
    return metadataOut;
}

void StructToArr(measurement measurementIn, uint8_t* arrOut) {
    uint32_t tmp = measurementIn.time / 60;

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

    measurementOut.time = tmp * 60;

    memcpy(&measurementOut.peatHeight, &arrIn[3], 2);
    memcpy(&measurementOut.waterHeight, &arrIn[5], 2);
    memcpy(&measurementOut.boxTemp, &arrIn[7], 2);
    memcpy(&measurementOut.groundTemp, &arrIn[9], 2);
    memcpy(&measurementOut.humidity, &arrIn[11], 2);

    return measurementOut;
}
