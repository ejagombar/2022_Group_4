#include "saved.h"

File DataFile;
File DeviceFile;
#define CSPIN A4

char* MACtoStr(const uint8_t* mac_addr) {
    static char macStr[18];
    snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
             mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
    return macStr;
}

void SDInterface::Init() {
    SD.begin(CSPIN);
}

void SDInterface::closeFiles() {
    if (openFile == DataFileOpen) {
        DataFile.close();
    }
    if (openFile == DeviceFileOpen) {
        DeviceFile.close();
    }
    openFile = NoneOpen;
}    

void SDInterface::AddDevice(SavedDevice deviceIn) {
    if (openFile == DataFileOpen) {
        DataFile.close();
        openFile = NoneOpen;
    }
    if (openFile == NoneOpen) {
        DeviceFile = SD.open("/DeviceFile.txt", FILE_APPEND);
    }
    if (DeviceFile) {
        Serial.println("file available");
    }
    DeviceFile.print(deviceIn.id);
    DeviceFile.print(",");
    DeviceFile.println(MACtoStr(deviceIn.macAddr));
    DeviceFile.close();
    openFile = NoneOpen;
    // savedDeviceArr[deviceCount] = deviceIn;
}

uint8_t SDInterface::GetDeviceCount() {
    return deviceCount;
}

SavedDevice SDInterface::GetDevice(uint8_t index) {
    // return savedDeviceArr[index];
}