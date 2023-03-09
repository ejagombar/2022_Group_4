/*
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/?s=esp-now
  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files.
  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
  Based on JC Servaye example: https://github.com/Servayejc/esp_now_sender/
*/

#include <Arduino.h>
#include <EEPROM.h>
#include <WiFi.h>
#include <esp_now.h>
#include <esp_wifi.h>

uint8_t serverAddress[] = {0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA};
// uint8_t serverAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

enum MessageType
{
    PairMessage,
    DataMessage,
};

struct struct_message
{
    const uint8_t msgType = PairMessage;
    uint8_t id;
    uint8_t height;
    uint8_t temp;
    uint16_t time;
};

struct struct_pairing
{ // new structure for pairing
    const uint8_t msgType = PairMessage;
    uint8_t id;
};

enum PairingStatus
{
    NOT_PAIRED,
    PAIR_REQUEST,
    PAIR_REQUESTED,
    PAIR_PAIRED,
};

struct_message myData; // data to send
struct_pairing pairingData;

PairingStatus pairingStatus = NOT_PAIRED;

MessageType messageType;
uint8_t myID = 0;

unsigned long currentMillis = millis();
unsigned long previousMillis = 0; // Stores last time temperature was published
const long interval = 500;       // Interval at which to publish sensor readings
unsigned long start;              // used to measure Pairing time

void printMAC(const uint8_t *mac_addr)
{
    char macStr[18];
    snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
             mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
    Serial.print(macStr);
}

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status)
{
    Serial.print("\r\nLast Packet Send Status:\t");
    Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Sent Success" : "Sent Fail");
}

void OnDataRecv(const uint8_t *mac_addr, const uint8_t *incomingData, int len)
{
    Serial.print("Packet received from: ");
    printMAC(mac_addr);
    Serial.println();
    Serial.print("data size = ");
    Serial.println(sizeof(incomingData));
    uint8_t type = incomingData[0];

    Serial.print("Type: ");
    Serial.println(type);
    Serial.print("myID: ");
    Serial.println(myID);

    Serial.print("(incomingData[0] == PairMessage): ");
    Serial.println((incomingData[0] == PairMessage));
    Serial.print(" (memcmp(mac_addr, serverAddress, 6) == 0): ");
    Serial.println((memcmp(mac_addr, serverAddress, 6) == 0));
    Serial.print("(myID == 0): ");
    Serial.println((myID == 0));

    if ((incomingData[0] == PairMessage) && (memcmp(mac_addr, serverAddress, 6) == 0))
    {
        if (myID == 0)
        {
            memcpy(&pairingData, incomingData, sizeof(pairingData));
            // if (pairingData.macAddr == serverAddress) {
            myID = pairingData.id;
            Serial.print("Mydata.Id changed to ");
            Serial.println(myData.id);
            //}
        }
        else
        {
            Serial.println("Pairing message receieved but ID already set");
        }
    }
}

void setup()
{
    Serial.begin(115200);
    Serial.println();
    Serial.print("Client Board MAC Address:  ");
    // Serial.println(WiFi.macAddress());
    WiFi.mode(WIFI_STA);
    // WiFi.disconnect();
    start = millis();

    if (esp_now_init() != ESP_OK)
    {
        Serial.println("Error initializing ESP-NOW");
    }

    esp_now_register_send_cb(OnDataSent);
    esp_now_register_recv_cb(OnDataRecv);

    esp_now_peer_info_t peerInfo = {};
    // register peer
    peerInfo.channel = 0;
    peerInfo.encrypt = false;
    // register first peer
    memcpy(peerInfo.peer_addr, serverAddress, 6);
    if (esp_now_add_peer(&peerInfo) != ESP_OK)
    {
        Serial.println("Failed to add peer");
        return;
    }

    myID = 0;
    pairingStatus = PAIR_REQUEST;
}

void loop()
{
    currentMillis = millis();
    if (currentMillis - previousMillis >= interval)
    {
        previousMillis = currentMillis;
        Serial.print("ID: ");
        Serial.println(myData.id);
        if (myID == 0)
        {
            pairingData.id = 0;

            esp_now_send(serverAddress, (uint8_t *)&pairingData, sizeof(pairingData));
            Serial.println("Attempting to send pairing message");
        }
        else
        {
            myData.temp = random(0, 100);
            myData.height = random(0, 100);
            myData.time = random(0, 1000);
            myData.id = myID;

            esp_err_t result = esp_now_send(serverAddress, (uint8_t *)&myData, sizeof(myData));
            Serial.println("Attempting to send data message");
        }
    }
}
