#include "espnow.h"
struct_pairing pairingData;
MessageState messageState;
uint8_t dataFrame[250];
uint8_t currentMAC[6];
uint8_t storedMAC[6];
bool storedMACSet;

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
        memcpy(&dataFrame, incomingData, len);
        memcpy(&currentMAC, mac_addr, 6);
        messageState = ProcessNewRequest;
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

void ESPNowInterface::addDevice(uint8_t* deviceMAC) {
    esp_now_peer_info_t peerInfo;
    memcpy(peerInfo.peer_addr, deviceMAC, 6);
    peerInfo.channel = 0;
    peerInfo.encrypt = false;
    if (esp_now_add_peer(&peerInfo) != ESP_OK) {
        Serial.println("Failed to add peer");
    }
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
    messageState = WaitingForMessage;
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

                deviceCount++;
                pairMsg.id = deviceCount;
                memcpy(storedMAC, &currentMAC, 6);
                storedMACSet = true;

                esp_now_send(currentMAC, (uint8_t *)&pairMsg, sizeof(pairMsg));
                messageState = WaitingForMessage;
            } else if (identicalMAC) {
                pairMsg.id = deviceCount;
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

uint8_t ESPNowInterface::getDeviceCount() {
    return deviceCount;
}

void ESPNowInterface::setDeviceCount(uint8_t count) {
    deviceCount = count;
}

uint8_t *ESPNowInterface::getCurrentMAC() {
    return currentMAC;
}

void ESPNowInterface::broadcastRequest(struct_RequestMessage request) {
    esp_now_send(NULL, (uint8_t *)&request, sizeof(request));
}

MessageState ESPNowInterface::getScanningState() {
    return messageState;
}

void ESPNowInterface::setScanningState(MessageState state) {
    messageState = state;
}

uint8_t *ESPNowInterface::getDataFrame() {
    return dataFrame;
}