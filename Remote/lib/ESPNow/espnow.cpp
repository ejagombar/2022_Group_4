#include "espnow.h"
struct_pairing pairingData;
MessageState messageState;
ScanningState scanningState;
uint8_t dataFrame[250];
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

void SetupOnDataRecv(const uint8_t *mac_addr, const uint8_t *incomingData, int len) {
    Serial.println();
    Serial.print(len);
    Serial.print(" bytes of data received from : ");
    printMAC(mac_addr);

    if ((incomingData[0] == PairMessage) && (messageState == WaitingForMessage)) {
        memcpy(&pairingData, incomingData, sizeof(pairingData));
        memcpy(&currentMAC, mac_addr, 6);
        messageState = ProcessNewRequest;
    }
}

void ScanOnDataRecv(const uint8_t *mac_addr, const uint8_t *incomingData, int len) {
    Serial.println();
    Serial.print(len);
    Serial.print(" bytes of data received from : ");
    printMAC(mac_addr);

    if (incomingData[0] == DataMessage) {
        memcpy(&dataFrame, incomingData, sizeof(dataFrame));
        memcpy(&currentMAC, mac_addr, 6);
        scanningState = RecievedData;
    }
}

//--------------------------------------------------------------------------------------------//

int ESPNowInterface::init() {
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

int ESPNowInterface::deinit() {
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

void ESPNowInterface::enableDeviceSetupCallback() {
    memset(currentMAC, 0, sizeof(currentMAC));
    memset(storedMAC, 0, sizeof(storedMAC));
    storedMACSet = false;
    messageState = WaitingForMessage;
    esp_now_register_recv_cb(SetupOnDataRecv);
    esp_now_register_send_cb(OnDataSent);
}

void ESPNowInterface::enableDeviceScanCallback() {
    memset(currentMAC, 0, sizeof(currentMAC));
    memset(storedMAC, 0, sizeof(storedMAC));
    memset(dataFrame, 0, sizeof(dataFrame));
    scanningState = BroadcastRequest;
    esp_now_register_recv_cb(ScanOnDataRecv);
    esp_now_register_send_cb(OnDataSent);
}

void ESPNowInterface::disableCallback() {
    esp_now_unregister_recv_cb();
    esp_now_unregister_send_cb();
}

MessageState ESPNowInterface::ProccessPairingMessage() {
    if (messageState == ProcessNewRequest) {
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
                messageState = WaitingForMessage;
            } else if (identicalMAC) {
                pairMsg.id = maxId;
                esp_now_send(currentMAC, (uint8_t *)&pairMsg, sizeof(pairMsg));
                messageState = WaitingForMessage;
            } else {
                // must be a message from a different peer
                messageState = WaitingForMessage;
            }

        } else if (identicalMAC) {
            messageState = Complete;
            Serial.println("Paired with monitor device");
        } else {
            Serial.println("HOW did we get here?");
        }
    }
    return messageState;
}

uint8_t ESPNowInterface::getMaxId() {
    return maxId;
}

uint8_t *ESPNowInterface::getCurrentMAC() {
    return currentMAC;
}

void ESPNowInterface::broadcastRequest(struct_RequestMessage request) {
    esp_now_send(NULL, (uint8_t *)&request, sizeof(request));
}

ScanningState ESPNowInterface::getScanningState() {
    return scanningState;
}

void ESPNowInterface::setScanningState(ScanningState state) {
    scanningState = state;
}

uint8_t *ESPNowInterface::getDataFrame() {
    return dataFrame;
}

// need to change from same mac to same id number
// need to first write function that will compact all the data into a 250 byte frame and send that from the monitor program.