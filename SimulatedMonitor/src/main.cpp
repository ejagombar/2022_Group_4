#include <Arduino.h>
#include <String.h>

#include "distanceSensor.h"
#include "espNowClient.h"
#include "pressureSensor.h"
#include "sampleBuffer.h"
#include "saved.h"
#include "temperatureSensor.h"

#define DEBUG 1
#define FIREBEETLE 1
//#define MSTACK 1

#ifdef MSTACK
#include <M5Core2.h>
#endif

const uint8_t buzzerOnCooldown = 240;
const uint8_t dataMsgCooldown = 50;
const uint8_t buzzerOffCooldown = 10;
const uint32_t espNowWaitTime = 250;

const uint8_t deepSleepTime = 5;
const uint8_t wakeUpsPerSample = 2;

DistanceSensor distanceSensor;
TemperatureSensor tempSensor;
PressureSensor pressureSensor;
SDInterface sd;
ESPNowClient espNow;

metadata deviceMetadata = {};

Error setupSensors() {
    Error err;
    String errorMsg;
    Error errOccured = NO_ERROR;

    err = distanceSensor.setup();
    if (err != NO_ERROR) {
        errOccured = FATAL_ERROR;
        errorMsg = String(": Distance sensor setup error: ") + String(err);
        sd.logError(errorMsg);
    };

    err = tempSensor.setup();
    if (err != NO_ERROR) {
        errOccured = FATAL_ERROR;
        errorMsg = String(": Temperature sensor setup error: ") + String(err);
        sd.logError(errorMsg);
    };

    err = pressureSensor.setup();
    if (err != NO_ERROR) {
        errOccured = FATAL_ERROR;
        errorMsg = String(": Pressure sensor setup error: ") + String(err);
        sd.logError(errorMsg);
    };
    return errOccured;
}

measurement takeSample() {
    measurement sample;
    Error err;
    String errorMsg;

    err = tempSensor.measure();
    if (err == NO_ERROR) {
        sample.boxTemp = (uint16_t)(tempSensor.getTemperature() * 100);
        sample.humidity = (uint16_t)(tempSensor.getHumidity() * 100);
    } else {
        errorMsg = String(": Temperature sensor read error: ") + String(err);
        sd.logError(errorMsg);
    }

    err = pressureSensor.measure();
    if (err == NO_ERROR) {
        sample.waterHeight = (uint16_t)(pressureSensor.getDepth());
        sample.groundTemp = (uint16_t)(pressureSensor.getTemperature() * 100);
    } else {
        errorMsg = String(": Pressure sensor read error: ") + String(err);
        sd.logError(errorMsg);
    }

    err = distanceSensor.measure();
    if (err == NO_ERROR) {
        sample.peatHeight = (uint16_t)distanceSensor.getResult();
    } else {
        errorMsg = String(": Distance sensor read error: ") + String(err);
        sd.logError(errorMsg);
    }

    sample.time = (uint32_t)random(0, 4294967295);

    return sample;
}

uint8_t setupDevice() {
    espNow.enableDeviceSetupCallback();
    unsigned long currentMillis = millis();
    unsigned long previousMillis = 0;
    const long interval = 500;  // Higher value = slower pairing but less battery drain and vice versa
    uint8_t id = 0;
    while (id == 0) {
        id = espNow.processPairingandGetID();

        currentMillis = millis();
        if (currentMillis - previousMillis >= interval) {
            previousMillis = currentMillis;
            espNow.sendPairRequest();
        }
    }

    sd.SetUp(id);

#ifdef DEBUG
    Serial.println("Paired!  Device ID: " + String(id));
#endif
    return id;
}

bool transmit() {
    uint16_t startlocation = deviceMetadata.transmittedNum;
    uint16_t sampleCount = deviceMetadata.sampleNum - startlocation;

    if (sampleCount > 19) {
        sampleCount = 19;
    }
    if (sampleCount == 0) {
        return false;
    }
    uint8_t packet[250] = {0};
    packet[0] = DataMessage;  // data packet
    packet[1] = deviceMetadata.ID;
    packet[2] = sampleCount;
    if (sd.getMeasurements(startlocation, &packet[3], sampleCount) == FATAL_ERROR) {
        Serial.println("Fatal error occured");
    }
    deviceMetadata.transmittedNum += sampleCount;
    sd.setMetadata(deviceMetadata);

    espNow.sendDataPacket(packet);
    return true;
}

void checkForBroadcast(uint8_t &repeat) {
    struct_RequestMessage requestMessage = espNow.processRemoteBroadcast();
    if ((requestMessage.monitorID == 0) || (requestMessage.monitorID == deviceMetadata.ID)) {
        if (requestMessage.requestData == true) {
            while (transmit() == true) {
                delay(10);
                repeat = dataMsgCooldown;
            }
        }
        if (requestMessage.enableBuzzer == true) {
#ifdef FIREBEETLE
            digitalWrite(LED_BUILTIN, HIGH);
#endif
#ifdef MSTACK
            M5.Lcd.clear();
            M5.Lcd.setCursor(0,0);
            M5.Lcd.print("Buzzer Enabled");
#endif
            repeat = buzzerOnCooldown;
        }
        if (requestMessage.disableBuzzer == true) {
#ifdef FIREBEETLE
            digitalWrite(LED_BUILTIN, LOW);
#endif
#ifdef MSTACK
            M5.Lcd.clear();
            M5.Lcd.setCursor(0,0);
            M5.Lcd.print("Buzzer Disabled");
#endif
            repeat = buzzerOffCooldown;
        }
    }
}

void setup() {
    Serial.begin(115200);
#ifdef MSTACK
    M5.begin();
    M5.Lcd.setTextSize(3);
#endif
}

void loop() {
#ifdef FIREBEETLE
    pinMode(LED_BUILTIN, OUTPUT);
#endif

    sd.Init();
    // sd.DeleteFiles();
    espNow.init();

    deviceMetadata = sd.getMetadata();

    if (deviceMetadata.ID == 0) {
        deviceMetadata.ID = setupDevice();
        espNow.sendPairConfirmation(deviceMetadata.ID);
        espNow.disableCallback();
    }

    espNow.enableRemoteBroadcastListener();
    // esp_sleep_enable_ext0_wakeup(GPIO_NUM_12, 0);

    deviceMetadata.wakeUpCount++;

#ifdef DEBUG
    Serial.println("Wakeup count: " + String(deviceMetadata.wakeUpCount));
#endif

    if (deviceMetadata.wakeUpCount >= wakeUpsPerSample) {
#ifdef DEBUG
        Serial.println("Taking sample");
#endif
        deviceMetadata.wakeUpCount = 0;

        setupSensors();

        uint8_t buf[13] = {0};
        measurement measure = takeSample();

        StructToArr(measure, buf);
        sd.saveMeasurement(deviceMetadata.sampleNum, buf);
        deviceMetadata.sampleNum++;
    }

    sd.setMetadata(deviceMetadata);

#ifdef DEBUG
    Serial.println("Checking for messages");
#endif

    uint8_t repeat = 1;

    while (repeat > 0) {
        repeat--;
        delay(espNowWaitTime);
        checkForBroadcast(repeat);
    }

    espNow.disableCallback();
#ifdef DEBUG
    Serial.println("Going to sleep");
#endif
// esp_deep_sleep_start();
#ifdef FIREBEETLE
    digitalWrite(LED_BUILTIN, LOW);
#endif
#ifdef MSTACK
    M5.Lcd.clear();
#endif
    delay(5000);
}