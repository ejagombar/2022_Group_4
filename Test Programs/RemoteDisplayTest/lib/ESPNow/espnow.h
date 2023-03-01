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

const uint8_t MACAddress[] = {0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA};

int initESPNow();