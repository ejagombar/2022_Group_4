#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>
#include <esp_wifi.h>

/*********
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp-now-many-to-one-esp32/

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*********/

// Structure example to receive data
// Must match the sender structure
typedef struct struct_message {
    uint16_t time;
    uint8_t id;
    uint8_t height;
    uint8_t temp;
} struct_message;

const uint8_t MACAddress[] = {0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA};

// Create a struct_message called myData
struct_message dataIn;

// callback function that will be executed when data is received
void OnDataRecv(const uint8_t *mac_addr, const uint8_t *incomingData, int len) {
    char macStr[18];
    Serial.print("Packet received from: ");
    snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
             mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
    Serial.println(macStr);
    memcpy(&dataIn, incomingData, sizeof(dataIn));
    // Serial.printf("Board ID %u: %u bytes\n", myData.id, len);
    // Serial.printf("Height: %d", myData.height);
    Serial.println();
}

void setup() {
    // Initialize Serial Monitor
    Serial.begin(115200);

    // Set device as a Wi-Fi Station
    WiFi.mode(WIFI_STA);

    // Init ESP-NOW
    if (esp_now_init() != ESP_OK) {
        Serial.println("Error initializing ESP-NOW");
        return;
    }

    esp_wifi_set_mac(WIFI_IF_STA, &MACAddress[0]);
    // Once ESPNow is successfully Init, we will register for recv CB to
    // get recv packer info
    esp_now_register_recv_cb(OnDataRecv);

    Serial.print("[NEW] ESP32 Board MAC Address:  ");
    Serial.println(WiFi.macAddress());
}

void loop() {
    delay(1000);
}
