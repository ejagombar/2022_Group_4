#include "espnow.h"

struct_message dataIn;
struct_message incomingReadings;
struct_pairing pairingData;

esp_now_peer_info_t slave;
int chan;

void printMAC(const uint8_t *mac_addr) {
    char macStr[18];
    snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
             mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
    Serial.print(macStr);
}

bool addPeer(const uint8_t *peer_addr) {  // add pairing
    memset(&slave, 0, sizeof(slave));
    const esp_now_peer_info_t *peer = &slave;
    memcpy(slave.peer_addr, peer_addr, 6);

    slave.channel = chan;  // pick a channel
    slave.encrypt = 0;     // no encryption
    // check if the peer exists
    bool exists = esp_now_is_peer_exist(slave.peer_addr);
    if (exists) {
        // Slave already paired.
        Serial.println("Already Paired");
        return true;
    } else {
        esp_err_t addStatus = esp_now_add_peer(peer);
        if (addStatus == ESP_OK) {
            // Pair success
            Serial.println("Pair success");
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

// void OnDataRecv(const uint8_t *mac_addr, const uint8_t *incomingData, int len) {
//     char macStr[18];
//     Serial.print("Packet received from: ");
//     snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x", mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
//     Serial.println(macStr);

//     memcpy(&dataIn, incomingData, sizeof(dataIn));
//     Serial.printf("Board ID %u: %u bytes\n", dataIn.id, len);
//     Serial.printf("height value: %d \n", dataIn.height);
//     Serial.printf("temp value: %d \n", dataIn.temp);
// }

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

void OnDataRecv(const uint8_t *mac_addr, const uint8_t *incomingData, int len) {
    Serial.print(len);
    Serial.print(" bytes of data received from : ");
    printMAC(mac_addr);
    String payload;
    uint8_t type = incomingData[0];  // first message byte is the type of message
    switch (type) {
        case DATA:
            memcpy(&incomingReadings, incomingData, sizeof(incomingReadings));

            Serial.println("Id: ");
            Serial.print(incomingReadings.id);
            Serial.println("Height:");
            Serial.print(incomingReadings.height);
            Serial.println("Temperature:");
            Serial.print(incomingReadings.temp);
            Serial.println();

            break;

        case PAIRING:
            memcpy(&pairingData, incomingData, sizeof(pairingData));
            Serial.println(pairingData.msgType);
            Serial.println(pairingData.id);
            Serial.print("Pairing request from: ");
            printMAC(mac_addr);
            Serial.println();
            if (pairingData.id > 0) {
                addPeer(mac_addr);
            }
            break;
    }
}