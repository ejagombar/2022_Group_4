#include "espNowClient.h"

struct_pairing pairingData;
struct_message myData;  // data to send
PairingState pairingState;

void printMAC(const uint8_t *mac_addr) {
    char macStr[18];
    snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
             mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
    Serial.print(macStr);
}

// void SetUpOnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
//     Serial.print("\r\nLast Packet Send Status:\t");
//     Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Sent Success" : "Sent Fail");
// }

void SetUpOnDataRecv(const uint8_t *mac_addr, const uint8_t *incomingData, int len) {
    Serial.print("Packet received from: ");
    printMAC(mac_addr);

    if ((incomingData[0] == PairMessage) && (memcmp(mac_addr, serverAddress, 6) == 0)) {
        memcpy(&pairingData, incomingData, sizeof(pairingData));
        pairingState = ProcessNewRequest;
    }
}

//=================================================================================================

void ESPNowClient::sendPairRequest() {
    pairingData.id = 0;

    esp_now_send(serverAddress, (uint8_t *)&pairingData, sizeof(pairingData));
    Serial.println("Attempting to send pairing message");
}

int ESPNowClient::processPairingandGetID() {
    if (pairingState == ProcessNewRequest) {
        int myID = pairingData.id;
        Serial.print("Mydata.Id changed to ");
        Serial.println(myID);
        pairingState = PairConfirmed;
        return myID;
    } else {
        return 0;
    }
}

void ESPNowClient::enableDeviceSetupCallback() {
    esp_now_register_recv_cb(SetUpOnDataRecv);
}

void ESPNowClient::disableCallback() {
    esp_now_unregister_recv_cb();
    esp_now_unregister_send_cb();
}

Error ESPNowClient::init() {
    WiFi.mode(WIFI_STA);
    if (esp_now_init() != ESP_OK) {
        Serial.println("Error initializing ESP-NOW");
        return FATAL_ERROR;
    }

    esp_now_peer_info_t serverInfo = {};
    serverInfo.channel = 0;
    serverInfo.encrypt = false;

    memcpy(serverInfo.peer_addr, serverAddress, 6);
    if (esp_now_add_peer(&serverInfo) != ESP_OK) {
        Serial.println("Failed to add server to peer list");
        return FATAL_ERROR;
    }

    return NO_ERROR;
}