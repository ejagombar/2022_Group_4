#include "espnow.h"
struct_message messageData;
struct_pairing pairingData;
esp_now_peer_info_t peerInfo;
PairingState pairingState;
uint64_t currentMAC = 0;
uint64_t storedMAC = 0;
uint8_t maxId = 0;
unsigned long previousMillis = 0;

void printMAC(const uint8_t *mac_addr) {
    char macStr[18];
    snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
             mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
    Serial.println(macStr);
}

void printMAC(uint8_t mac_addr[6]) {
    char macStr[18];
    snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
             mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
    Serial.println(macStr);
}

uint64_t convertMac(const uint8_t *mac_addr) {
    uint64_t result = 0;
    for (int i = 0; i < 6; ++i) {
        result |= static_cast<uint64_t>(mac_addr[i]) << (5 - i) * 8;
    }
    return result;
}

void uint64_to_mac(uint64_t value, uint8_t *mac) {
    for (int i = 0; i < 6; ++i) {
        mac[i] = static_cast<uint8_t>((value >> (5 - i) * 8) & 0xFF);
    }
}

//--------------------------------------------------------------------------------------------//

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
    Serial.print("\r\nLast Packet Send Status:\t");
    Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

void OnDataRecv(const uint8_t *mac_addr, const uint8_t *incomingData, int len) {
    Serial.println();
    Serial.print(len);
    Serial.print(" bytes of data received from : ");
    printMAC(mac_addr);

    if (pairingState == WaitingForPair) {
        memcpy(&pairingData, incomingData, sizeof(pairingData));
        currentMAC = convertMac(mac_addr);
        pairingState = RecievedPairRequest;
    } else if (pairingState == IDSent) {
        pairingState = SendID;
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
    pairingState = WaitingForPair;
    esp_now_register_recv_cb(OnDataRecv);
    esp_now_register_send_cb(OnDataSent);
}

void EPSNowInterface::enableDeviceScanCallback() {
    esp_now_register_recv_cb(OnDataRecv);
}

void EPSNowInterface::disableCallback() {
    esp_now_unregister_recv_cb();
    esp_now_unregister_send_cb();
}

void EPSNowInterface::sendTestMessage() {
    const uint8_t mac[] = {0x08, 0x3a, 0xf2, 0x43, 0x98, 0x0c};
    struct_pairing pairingDataOut;
    pairingDataOut.newId = maxId;
    Serial.print("Trying to send message: ");
    Serial.println(esp_now_send(mac, (uint8_t *)&pairingDataOut, sizeof(pairingDataOut)));
}

void EPSNowInterface::ProccessPairingMessage() {
    unsigned long currentMillis = millis();
    if ((pairingState == RecievedPairRequest) || (pairingState == SendID)) {
        previousMillis = currentMillis;
        Serial.print("pairing id");
        Serial.println(pairingData.id);
        if (pairingData.id == 0) {
            struct_pairing pairingDataOut;
            uint8_t mac[6];
            uint64_to_mac(storedMAC, mac);
            if (pairingState != SendID) {
                // we already have this device but need to send ID to peer again

                peerInfo.channel = 0;
                peerInfo.encrypt = false;
                memcpy(peerInfo.peer_addr, mac, 6);
                printMAC(mac);
                if (esp_now_add_peer(&peerInfo) != ESP_OK) {
                    Serial.println("Failed to add peer");
                    return;
                }

                maxId++;
                pairingDataOut.newId = maxId;
                storedMAC = currentMAC;

                Serial.print("New ID issued to peer. Number:");
                Serial.println(maxId);

            } else {
                // new device so new id number needed
                Serial.print("Device missed so sending ID to peer again: ");
                Serial.println(maxId);
            }

            esp_now_send(mac, (uint8_t *)&pairingDataOut, sizeof(pairingDataOut));
            pairingState = IDSent;

        } else if (currentMAC = storedMAC) {
            if (pairingData.id == maxId) {
                Serial.println("Pairing great success");
                pairingState = PairConfirmed;
            }
        }
    }
}

bool addPeer(const uint8_t *peer_addr) {
    return true;
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
