#include "espnow.h"

struct_message messageData;
struct_pairing pairingData;
esp_now_peer_info_t slave;

void printMAC(const uint8_t *mac_addr) {
    char macStr[18];
    snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
             mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
    Serial.print(macStr);
}

bool addPeer(const uint8_t *peer_addr) {
    memset(&slave, 0, sizeof(slave));
    const esp_now_peer_info_t *peer = &slave;
    memcpy(slave.peer_addr, peer_addr, 6);

    slave.channel = 0;
    slave.encrypt = false;

    bool exists = esp_now_is_peer_exist(slave.peer_addr);
    if (exists) {
        Serial.println("Already Paired");
        return true;
    } else {
        esp_err_t addStatus = esp_now_add_peer(peer);
        if (addStatus == ESP_OK) {

            Serial.println("Pair success");
            struct_pairing pairingDataOut;
            pairingDataOut.newId = 2;

            esp_now_send(peer_addr, (uint8_t *)&pairingDataOut, sizeof(pairingDataOut));
            return true;
        } else {
            Serial.println("Pair failed");
            return false;
        }
    }
}

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
    Serial.print("\r\nLast Packet Send Status:\t");
    Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

void OnDataRecv(const uint8_t *mac_addr, const uint8_t *incomingData, int len) {
    Serial.print(len);
    Serial.print(" bytes of data received from : ");
    printMAC(mac_addr);
    String payload;
    uint8_t type = incomingData[0];  // first message byte is the type of message
    if (type == 0) {
        memcpy(&pairingData, incomingData, sizeof(pairingData));

        Serial.print("Pairing request from: ");
        printMAC(mac_addr);
        addPeer(mac_addr);
    }
}

//--------------------------------------------------------------------------------------------//

int EPSNowInterface::init() {
    if (!WiFi.mode(WIFI_STA)) {
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

int EPSNowInterface::deinit() {
    if (esp_now_deinit() != ESP_OK) {
        Serial.println("Error deinitializing ESP-NOW");
        return ESPNowFail;
    }

    if (!WiFi.mode(WIFI_OFF)) {
        Serial.println("Error deinitializing Wifi");
        return WifiModeFail;
    }

    Serial.print("ESP Now has been deinitialized");

    return Success;
}

void EPSNowInterface::enableDeviceSetupCallback() {
    esp_now_register_recv_cb(OnDataRecv);
}

void EPSNowInterface::enableDeviceScanCallback() {
    esp_now_register_recv_cb(OnDataRecv);
}

void EPSNowInterface::disableCallback() {
    esp_now_unregister_recv_cb();
}


    //  else {
    //     memcpy(&messageData, incomingData, sizeof(messageData));

    //     Serial.print("Id: ");
    //     Serial.println(messageData.id);
    //     Serial.println("Height:");
    //     Serial.println(messageData.height);
    //     Serial.println("Temperature:");
    //     Serial.println(messageData.temp);
    //     Serial.println();
    // }