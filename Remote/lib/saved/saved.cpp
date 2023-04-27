#include "saved.h"

File CurrentFile;
#define CSPIN A4

char* MACtoStr(const uint8_t* mac_addr) {
    static char macStr[18];
    snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
             mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
    return macStr;
}

bool SDInterface::Init() {
    bool init = SD.begin(CSPIN);
    if (!SD.exists(deviceFileName)) {
        CurrentFile = SD.open(deviceFileName, FILE_WRITE);
        CurrentFile.seek(0);
        CurrentFile.write(0);
        deviceCount = 0;
    } else {
        CurrentFile = SD.open(deviceFileName, FILE_READ);
        CurrentFile.seek(0);
        CurrentFile.read(&deviceCount, sizeof(deviceCount));
    }
    CurrentFile.close();
    return init;
}

void SDInterface::DeleteFiles() {
    SD.remove(deviceFileName);
    deviceCount = 0;
}

void SDInterface::AddDevice(SavedDevice deviceIn) {
    CurrentFile = SD.open(deviceFileName, FILE_WRITE);
    if (CurrentFile) {
        CurrentFile.seek(1 + (sizeof(deviceIn) * deviceCount));
        CurrentFile.write((uint8_t*)&deviceIn, sizeof(deviceIn));

        deviceCount++;
        CurrentFile.seek(0);
        CurrentFile.write(&deviceCount, sizeof(deviceCount));
        CurrentFile.close();
    }
}

uint8_t SDInterface::GetDeviceCount() {
    return deviceCount;
}

SavedDevice SDInterface::GetDevice(uint8_t index) {
    SavedDevice deviceOut = {};
    CurrentFile = SD.open(deviceFileName, FILE_READ);
    if (CurrentFile) {
        CurrentFile.seek(1 + (sizeof(SavedDevice) * index));
        CurrentFile.read((uint8_t*)&deviceOut, sizeof(deviceOut));
        CurrentFile.close();
    }
    return deviceOut;
}

void SDInterface::openMonitorFile(char* fileName) {
    CurrentFile = SD.open(fileName, FILE_APPEND);
}

void SDInterface::closeFile() {
    CurrentFile.close();
}

void SDInterface::printPacket(measurement measurementIn, char* dateTime) {
    CurrentFile.print(dateTime);
    CurrentFile.print(", ");
    CurrentFile.print(measurementIn.peatHeight);
    CurrentFile.print(", ");
    CurrentFile.print(measurementIn.waterHeight);
    CurrentFile.print(", ");
    CurrentFile.print((float)measurementIn.boxTemp/100.0);
    CurrentFile.print(", ");
    CurrentFile.print((float)measurementIn.groundTemp/100.0);
    CurrentFile.print(", ");
    CurrentFile.print((float)measurementIn.humidity/100.0);
    CurrentFile.println("");
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