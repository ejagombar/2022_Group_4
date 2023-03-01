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
    WiFi.mode(WIFI_STA);

    esp_wifi_set_mac(WIFI_IF_STA, &MACAddress[0]);

    if (esp_now_init() != ESP_OK) {
        Serial.println("esp_now_init() failed");
        return ESP_FAIL;
    }

    Serial.print("[NEW] ESP32 Board MAC Address:  ");
    Serial.println(WiFi.macAddress());

    esp_now_register_recv_cb(OnDataRecv);
    // esp_now_register_send_cb(OnDataSent);
}
