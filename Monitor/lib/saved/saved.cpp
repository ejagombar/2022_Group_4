#include "saved.h"

File DataFile;
File MonitorDataFile;

void SDInterface::Init() {
    SD.begin(CSPIN);
}

void SDInterface::closeFiles() {
    if (openFile == DataFileOpen) {
        DataFile.close();
    }
    if (openFile == MonitorDataFileOpen) {
        MonitorDataFile.close();
    }
    openFile = NoneOpen;
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
