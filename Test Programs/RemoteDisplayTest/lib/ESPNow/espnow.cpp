#include "espnow.h"
struct_message messageData;
struct_pairing pairingData;
PairingState pairingState;
uint8_t currentMAC[6];
uint8_t storedMAC[6];
bool storedMACSet;

uint8_t maxId = 0;

void printMAC(const uint8_t *mac_addr) {
    char macStr[18];
    snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
             mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
    Serial.println(macStr);
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

    if ((incomingData[0] == PairMessage) && (pairingState == WaitingForPairRequest)) {
        memcpy(&pairingData, incomingData, sizeof(pairingData));
        memcpy(&currentMAC, mac_addr, 6);
        pairingState = ProcessNewRequest;
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
    memset(currentMAC, 0, sizeof(currentMAC));
    memset(storedMAC, 0, sizeof(storedMAC));
    storedMACSet = false;
    pairingState = WaitingForPairRequest;
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

PairingState EPSNowInterface::ProccessPairingMessage() {
    if (pairingState == ProcessNewRequest) {
        esp_now_peer_info_t peerInfo = {};
        struct_pairing pairMsg = {};
        bool identicalMAC = (memcmp(storedMAC, currentMAC, sizeof(storedMAC)) == 0);
        if (pairingData.id == 0) {
            if (storedMACSet == false) {
                peerInfo.channel = 0;
                peerInfo.encrypt = false;
                memcpy(peerInfo.peer_addr, currentMAC, 6);
                printMAC(currentMAC);
                Serial.println("added to peer list and issued ID number");
                if (esp_now_add_peer(&peerInfo) != ESP_OK) {
                    Serial.println("Failed to add peer");
                }

                maxId++;
                pairMsg.id = maxId;
                memcpy(storedMAC, &currentMAC, 6);
                storedMACSet = true;

                esp_now_send(currentMAC, (uint8_t *)&pairMsg, sizeof(pairMsg));
                pairingState = WaitingForPairRequest;
            } else if (identicalMAC) {
                pairMsg.id = maxId;
                esp_now_send(currentMAC, (uint8_t *)&pairMsg, sizeof(pairMsg));
                pairingState = WaitingForPairRequest;
            } else {
              // must be a message from a different peer
              pairingState = WaitingForPairRequest;
            }

        } else if (identicalMAC) {
            pairingState = PairConfirmed;
            Serial.println("Paired with monitor device");
        } else {
            Serial.println("How did we get here?");
        }
    }
    return pairingState;
}

