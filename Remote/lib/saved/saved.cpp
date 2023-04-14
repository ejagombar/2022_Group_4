#include "saved.h"

File CurrentFile;
#define CSPIN A4

char* MACtoStr(const uint8_t* mac_addr) {
    static char macStr[18];
    snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
             mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
    return macStr;
}

void SDInterface::Init() {
    SD.begin(CSPIN);
    if (!SD.exists(deviceFileName)) {
        CurrentFile = SD.open(deviceFileName, FILE_WRITE);
        CurrentFile.seek(0);
        CurrentFile.write(0);
        deviceCount = 0;
    } else {
        CurrentFile = SD.open(deviceFileName, FILE_WRITE);
        CurrentFile.seek(0);
        CurrentFile.read(&deviceCount, sizeof(deviceCount));
    }
    CurrentFile.close();
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
        CurrentFile.write(deviceCount);
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