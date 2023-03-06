#include "saved.h"

void Saved::AddDevice(SavedDevice deviceIn) {
    deviceCount++;
    savedDeviceArr[deviceCount] = deviceIn;
}

uint8_t Saved::GetDeviceCount() {
    return deviceCount;
}

SavedDevice Saved::GetDevice(uint8_t index) {
    return savedDeviceArr[index];
}