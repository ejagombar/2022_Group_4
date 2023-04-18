#include <Arduino.h>
#include <RTClib.h>
#include <SPI.h>
#include <String.h>
#include <Wire.h>

#include "distanceSensor.h"
#include "espNowClient.h"
#include "pressureSensor.h"
#include "sampleBuffer.h"
#include "saved.h"
#include "temperatureSensor.h"

#define PressureLidarVcc1 D11
#define PressureLidarVcc2 D12
#define TempVcc D10
#define SDVcc D6
#define BuzzerVcc D7
#define INTERRUPT_PIN D13

// #define DEBUG 1

#define SECONDS_FROM_1970_TO_2023 1672531200

const uint8_t buzzerOnCooldown = 240;
const uint8_t dataMsgCooldown = 80;
const uint8_t buzzerOffCooldown = 0;
const uint32_t espNowWaitTime = 250;

const uint8_t deepSleepTime = 5;
const uint8_t wakeUpsPerSample = 3;

DistanceSensor distanceSensor;
TemperatureSensor tempSensor;
PressureSensor pressureSensor;
SDInterface sd;
ESPNowClient espNow;
DateTime currentTime;

RTC_PCF8523 Rtc;

volatile bool alarm_triggered = false;
metadata deviceMetadata = {};

char time_format_buf[] = "DD/MM/YY hh:mm:00";

void alarmISR() {
    alarm_triggered = true;
}

void setAlarmInterval(uint8_t interval) {
    alarm_triggered = false;

    DateTime current_time = Rtc.now();

    uint8_t alarmMin = ceil(float(current_time.minute() + 1) / interval) * interval;
    if (alarmMin >= 60) {
        alarmMin = 0;
    }
    DateTime alarm_time(current_time.year(), current_time.month(), current_time.day(), current_time.hour(), alarmMin, 0);

    Rtc.enableAlarm(alarm_time, PCF8523_AlarmMinute);

#ifdef DEBUG
    Serial.println(String("Alarm Time: ") + alarm_time.toString(time_format_buf));
#endif
}

Error setupSensors(DateTime currentTime) {
    Error err;
    String errorMsg;
    Error errOccured = NO_ERROR;

    err = distanceSensor.setup();
    if (err != NO_ERROR) {
        errOccured = FATAL_ERROR;
        errorMsg = currentTime.toString(time_format_buf) + String(": Distance sensor setup error: ") + String(err);
        sd.logError(errorMsg);
    };

    err = tempSensor.setup();
    if (err != NO_ERROR) {
        errOccured = FATAL_ERROR;
        errorMsg = currentTime.toString(time_format_buf) + String(": Temperature sensor setup error: ") + String(err);
        sd.logError(errorMsg);
    };

    err = pressureSensor.setup();
    if (err != NO_ERROR) {
        errOccured = FATAL_ERROR;
        errorMsg = currentTime.toString(time_format_buf) + String(": Pressure sensor setup error: ") + String(err);
        sd.logError(errorMsg);
    };
    return errOccured;
}

measurement takeSample(DateTime currentTime) {
    measurement sample = {0, 0, 0, 0, 0, 0};
    Error err;
    String errorMsg;

    err = tempSensor.measure();
    if (err == NO_ERROR) {
        sample.boxTemp = (uint16_t)(tempSensor.getTemperature() * 100);
        sample.humidity = (uint16_t)(tempSensor.getHumidity() * 100);
    } else {
        errorMsg = currentTime.toString(time_format_buf) + String(": Temperature sensor read error: ") + String(err);
        sd.logError(errorMsg);
    }

    err = pressureSensor.measure();
    if (err == NO_ERROR) {
        sample.waterHeight = (uint16_t)(pressureSensor.getDepth() * 100);
        sample.groundTemp = (uint16_t)(pressureSensor.getTemperature() * 100);
    } else {
        errorMsg = currentTime.toString(time_format_buf) + String(": Pressure sensor read error: ") + String(err);
        sd.logError(errorMsg);
    }

    err = distanceSensor.measure();
    if (err == NO_ERROR) {
        sample.peatHeight = (uint16_t)distanceSensor.getResult();
    } else {
        errorMsg = currentTime.toString(time_format_buf) + String(": Distance sensor read error: ") + String(err);
        sd.logError(errorMsg);
    }

    sample.time = (currentTime.unixtime() - SECONDS_FROM_1970_TO_2023);  // number of seconds since 01/01/2023

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
                delay(100);
                repeat = dataMsgCooldown;
            }
        }
        if (requestMessage.enableBuzzer == true) {
            digitalWrite(BuzzerVcc, HIGH);
            repeat = buzzerOnCooldown;
        }
        if (requestMessage.disableBuzzer == true) {
            digitalWrite(BuzzerVcc, LOW);
            repeat = buzzerOffCooldown;
        }
    }
}

void setup() {
    Serial.begin(115200);

    pinMode(PressureLidarVcc1, OUTPUT);
    pinMode(PressureLidarVcc2, OUTPUT);
    pinMode(TempVcc, OUTPUT);
    pinMode(SDVcc, OUTPUT);
    pinMode(BuzzerVcc, OUTPUT);

    digitalWrite(SDVcc, HIGH);
    digitalWrite(PressureLidarVcc1, HIGH);
    digitalWrite(PressureLidarVcc2, HIGH);
    digitalWrite(TempVcc, HIGH);

    pinMode(INTERRUPT_PIN, INPUT_PULLUP);
    attachInterrupt(INTERRUPT_PIN, alarmISR, FALLING);

    if (!Rtc.begin()) {
        Serial.println("Couldn't find RTC");
        return;
    }

    // Rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

    sd.Init();
    // sd.DeleteFiles();
    espNow.init();

    currentTime = Rtc.now();
    deviceMetadata = sd.getMetadata();

    //deviceMetadata.transmittedNum = 0;

    if (deviceMetadata.ID == 0) {
        deviceMetadata.ID = setupDevice();
        espNow.sendPairConfirmation(deviceMetadata.ID);
        espNow.disableCallback();
    }

    espNow.enableRemoteBroadcastListener();
    esp_sleep_enable_ext0_wakeup(GPIO_NUM_12, 0);
    setAlarmInterval(deepSleepTime);  // to wake the esp

    deviceMetadata.wakeUpCount++;

#ifdef DEBUG
    Serial.println("Wakeup count: " + String(deviceMetadata.wakeUpCount));
#endif

    if (deviceMetadata.wakeUpCount >= wakeUpsPerSample) {
#ifdef DEBUG
        Serial.println("Taking sample");
#endif
        deviceMetadata.wakeUpCount = 0;

        setupSensors(currentTime);

        uint8_t buf[13] = {0};
        measurement measure = takeSample(currentTime);

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

    digitalWrite(SDVcc, LOW);
    digitalWrite(PressureLidarVcc1, LOW);
    digitalWrite(PressureLidarVcc2, LOW);
    digitalWrite(TempVcc, LOW);

    espNow.disableCallback();
#ifdef DEBUG
    Serial.println("Going to sleep");
#endif
    esp_deep_sleep_start();
}

void loop() {
}