#include "espnow.h"

struct_message myData;

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
    Serial.print("\r\nLast Packet Send Status:\t");
    Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

void OnDataRecv(const uint8_t *mac_addr, const uint8_t *incomingData, int len) {
    char macStr[18];
    Serial.print("Packet received from: ");
    snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x", mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
    Serial.println(macStr);

    memcpy(&myData, incomingData, sizeof(myData));
    Serial.printf("Board ID %u: %u bytes\n", myData.id, len);
    Serial.printf("height value: %d \n", myData.height);
    Serial.printf("temp value: %d \n", myData.temp);
}

int initESPNow() {
    if (WiFi.mode(WIFI_STA) == false) {
        Serial.println("Error initializing Wifi");
        return WifiModeFail;
    }

    if (esp_now_init() != ESP_OK) {
        Serial.println("Error initializing ESP-NOW");
        return ESPNowFail;
    }

    esp_wifi_set_mac(WIFI_IF_STA, &MACAddress[0]);

    Serial.print("ESP Now has been initialized. \nMAC Address: ");
    Serial.println(WiFi.macAddress());

    return Success;
}

int deinitESPNow() {
    if (esp_now_deinit() != ESP_OK) {
        Serial.println("Error deinitializing ESP-NOW");
        return ESPNowFail;
    }

    if (WiFi.mode(WIFI_OFF) == false) {
        Serial.println("Error deinitializing Wifi");
        return WifiModeFail;
    }

    Serial.print("ESP Now has been deinitialized.");

    return Success;
}

void setupRecieveData() {
    esp_now_register_recv_cb(OnDataRecv);
}

void closeRecieveData() {
    esp_now_unregister_recv_cb();
}