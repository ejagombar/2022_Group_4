#include "saved.h"

Error SDInterface::Init() {
    if (!SD.begin(CSPIN)) {
        return -1;
    } else {
        return 0;
    }
}

void SDInterface::openFileWrite(OpenFile choice) {
    openFile = choice;
    if (openFile == DataFileOpen) {
        CurrentFile = SD.open("/logdata.bin", FILE_WRITE);
    }
    if (openFile == MonitorDataFileOpen) {
        CurrentFile = SD.open("/metadata.bin", FILE_WRITE);
    }
}

void SDInterface::closeFile() {
    CurrentFile.close();
    openFile = NoneOpen;
}

Error SDInterface::saveMeasurement(const uint8_t* arrIn) {
    if (openFile == NoneOpen) {
        openFileWrite(DataFileOpen);
    }
    if (CurrentFile) {
        CurrentFile.seek(CurrentFile.size());
        CurrentFile.write(arrIn, 13);
        return NO_ERROR;
    } else {
        return FATAL_ERROR;
    }
}

Error SDInterface::getMeasurements(int index, uint8_t* arrOut, int sampleCount) {
    if (openFile == NoneOpen) {
        openFileWrite(DataFileOpen);
    }
    if (CurrentFile) {
        CurrentFile.seek(index * 13);
        CurrentFile.read(arrOut, sampleCount * 13);
        return NO_ERROR;
    } else {
        return FATAL_ERROR;
    }
}

void SDInterface::clearFile() {
    if (SD.exists("/logdata.bin")) {
        SD.remove("/logdata.bin");
        Serial.println("File Removed");
    }
}

// void SDInterface::AddDevice(SavedDevice deviceIn) {
//     if (openFile == DataFileOpen) {
//         DataFile.close();
//         openFile = NoneOpen;
//     }
//     if (openFile == NoneOpen) {
//         MonitorDataFile = SD.open("/DeviceFile.txt", FILE_APPEND);
//     }
//     if (MonitorDataFile) {
//         Serial.println("file available");
//     }
//     MonitorDataFile.print(deviceIn.id);
//     MonitorDataFile.print(",");
//     MonitorDataFile.close();
//     openFile = NoneOpen;
//     // savedDeviceArr[deviceCount] = deviceIn;
// }
