#include "espnow.h"
struct_message messageData;
struct_pairing pairingData;
PairingState pairingState;
uint8_t currentMAC[6];
uint8_t storedMAC[6];

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

    if (incomingData[0] == PairMessage) {
        if (pairingState == WaitingForPairRequest) {
            memcpy(&pairingData, incomingData, sizeof(pairingData));
            memcpy(&currentMAC, mac_addr, 6);
            pairingState = InitNewPairing;
        } else if (pairingState == WaitingForConfirmation) {
            memcpy(&pairingData, incomingData, sizeof(pairingData));
            memcpy(&currentMAC, mac_addr, 6);
            pairingState = ProcessNewRequest;
        }
    }
}

void EPSNowInterface::ProccessPairingMessage() {
    struct_pairing pairMsg;
    esp_now_peer_info_t peerInfo;

    if (pairingState == InitNewPairing) {
        if (pairingData.id == 0) {

            peerInfo.channel = 0;
            peerInfo.encrypt = false;
            memcpy(peerInfo.peer_addr, currentMAC, 6);
            printMAC(currentMAC);
            if (esp_now_add_peer(&peerInfo) != ESP_OK) {
                Serial.println("Failed to add peer");
            }

            maxId++;
            pairMsg.id = maxId;
            memcpy(storedMAC, &currentMAC, 6);

            // assign new id if new mac else send the current id
            // send the message and wait
        } else {
            // confirmed pairing
        }

        Serial.print("New ID issued to peer. Number:");
        Serial.println(maxId);

    } else {
        // new device so new id number needed
        Serial.print("Device missed so sending ID to peer again: ");
        Serial.println(maxId);
    }

    esp_now_send(currentMAC, (uint8_t *)&pairMsg, sizeof(pairMsg));
    pairingState = IDSent;
}
else {
    if (memcmp(storedMAC, currentMAC, sizeof(storedMAC))) {
        Serial.println("Pairing great success");
        pairingState = PairConfirmed;
    }
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
    pairingState = WaitingForMessage;
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

//  else {
//     memcpy(&messageData, incomingData, sizeof(messageData));

//     Serial.print("Id: ");
//     Serial.println(messageData.id);
//     Serial.println("Height:");
//     Serial.println(messageData.height);
//     Serial.println("Temperature:");
//     Serial.println(messageData.temp);
//     Serial.println();
