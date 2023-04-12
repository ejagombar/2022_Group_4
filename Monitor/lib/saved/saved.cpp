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


Error SDInterface::saveMeasurement(const uint8_t *arrIn) {

    
    measurement measure = {16896, 100, 128, 2332, 0xFF, 0xAAAA};

    //StructToArr(measure, buf);
    // if (openFile == NoneOpen) {
    //     openFileWrite(DataFileOpen);
    // }
    // if (CurrentFile) {
    //     CurrentFile.seek(CurrentFile.size());
    //     Serial.println("Size of file: " + String(CurrentFile.size()));
    //     CurrentFile.write((const uint8_t *)&arrTest, 13);
    //     CurrentFile.close();
    //     return NO_ERROR;
    // } else {
    //     return FATAL_ERROR;
    // }

    Serial.println("Fetching pre-made data");
    CurrentFile = SD.open(filename, FILE_WRITE);
    // Serial.print("Length: "); Serial.println(sizeof(myData) / 4);
    // for (int i = 0; i < sizeof(myData) / 4; i++) {
    //     myData[i].x = i + 1;
    //     myData[i].y = (i + 1) * 2;
    // }
    // CurrentFile.write((const uint8_t *)&myData, sizeof(myData));
    CurrentFile.write(arrIn, 13);
    CurrentFile.close();
    return NO_ERROR;
}

Error SDInterface::getMeasurements(int index, uint8_t *arrOut, int sampleCount) {
    // if (openFile == NoneOpen) {
    //     openFileWrite(DataFileOpen);
    // }
    // if (CurrentFile) {
    //     CurrentFile.seek(index * 13);
    //     CurrentFile.read(arrOut, sampleCount * 13);
    //     return NO_ERROR;
    // } else {
    //     return FATAL_ERROR;
    // }
    uint8_t Data[13]  =  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

    Serial.println("Reading Data");
    CurrentFile = SD.open(filename, FILE_READ);
    CurrentFile.read(arrOut, 13);
    //measurement out = ArrToStruct(Data);

    // Serial.print("timeOut: ");
    // Serial.println(out.time);
    // Serial.print("peatHeight: ");
    // Serial.println(out.peatHeight);
    // Serial.print("waterHeight: ");
    // Serial.println(out.waterHeight);
    // Serial.print("boxTemp: ");
    // Serial.println(out.boxTemp);
    // Serial.print("groundTemp: ");
    // Serial.println(out.groundTemp);
    // Serial.print("humidity: ");
    // Serial.println(out.humidity);
    // CurrentFile.seek(index * 13);
    // CurrentFile.read(arrOut, sampleCount * 13);
    // Serial.print("x");
    // Serial.print("\t");
    // Serial.println("y");
    // for (int i = 0; i < sizeof(Data) / 4; i++) {
    //     Serial.print(Data[i].x);
    //     Serial.print("\t");
    //     Serial.println(Data[i].y);
    // }
    CurrentFile.close();
    return NO_ERROR;
}

void SDInterface::clearFile() {
    if (SD.exists("/logdata.bin")) {
        SD.remove("/logdata.bin");
        Serial.println("File Removed");
    }
}

// File dataFile;

// void makeData() {
//     Serial.println("Fetching pre-made data");
//     dataFile = SD.open(filename, FILE_WRITE);
//     // Serial.print("Length: "); Serial.println(sizeof(myData) / 4);
//     for (int i = 0; i < sizeof(myData) / 4; i++) {
//         myData[i].x = i + 1;
//         myData[i].y = (i + 1) * 2;
//     }
//     dataFile.write((const uint8_t *)&myData, sizeof(myData));
//     dataFile.close();
// }

// void readIt() {
//     Serial.println("Reading Data");
//     dataFile = SD.open(filename, FILE_READ);
//     dataFile.read((uint8_t *)&Data, sizeof(Data));
//     Serial.print("x");
//     Serial.print("\t");
//     Serial.println("y");
//     for (int i = 0; i < sizeof(Data) / 4; i++) {
//         Serial.print(Data[i].x);
//         Serial.print("\t");
//         Serial.println(Data[i].y);
//     }
//     dataFile.close();
// }

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
