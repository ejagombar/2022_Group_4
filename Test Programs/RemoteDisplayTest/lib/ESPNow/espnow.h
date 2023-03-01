#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>
#include <esp_wifi.h>

typedef struct struct_message {
  uint16_t time;
  uint8_t id;
  uint8_t height;
  uint8_t temp;
}struct_message;

enum InitStatus {
    Success = 0,
    WifiModeFail = -1,
    ESPNowFail = -2
};

const uint8_t MACAddress[] = {0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA};

int initESPNow();
int deinitESPNow();
void setupRecieveData();
void closeRecieveData();