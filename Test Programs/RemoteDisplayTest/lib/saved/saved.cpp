#include "saved.h"

void Saved::AddDevice(SavedDevice device) {
    deviceCount++;
    savedDeviceArr[deviceCount - 1] = device;
}

SavedDevice Saved::GetDevice(uint8_t index) {
    return savedDeviceArr[index];
}

uint8_t Saved::GetDeviceCount() {
    return deviceCount;
}