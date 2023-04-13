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

#define SECONDS_FROM_1970_TO_2023 1672531200

#define INTERRUPT_PIN D13

DistanceSensor distanceSensor;
TemperatureSensor tempSensor;
PressureSensor pressureSensor;
SDInterface sd;
ESPNowClient espNow;
DateTime currentTime;

RTC_PCF8523 Rtc;

volatile bool alarm_triggered = false;
uint8_t deviceID = 0;

char time_format_buf[] = "YYYY-MM-DDThh:mm:00";

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

    char alarm_time_buf[] = "YYYY-MM-DDThh:mm:00";
    Serial.println(String("Alarm Time: ") + alarm_time.toString(alarm_time_buf));
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
        Serial.println(errorMsg);
    };

    err = tempSensor.setup();
    if (err != NO_ERROR) {
        errOccured = FATAL_ERROR;
        errorMsg = currentTime.toString(time_format_buf) + String(": Temperature sensor setup error: ") + String(err);
        sd.logError(errorMsg);
        Serial.println(errorMsg);
    };

    err = pressureSensor.setup();
    if (err != NO_ERROR) {
        errOccured = FATAL_ERROR;
        errorMsg = currentTime.toString(time_format_buf) + String(": Pressure sensor setup error: ") + String(err);
        sd.logError(errorMsg);
        Serial.println(errorMsg);
    };
    return errOccured;
}

measurement takeSample(DateTime currentTime) {
    measurement sample;
    Error err;
    String errorMsg;
    char time_format_buf[] = "YYYY-MM-DDThh:mm:00";

    err = tempSensor.measure();
    if (err == NO_ERROR) {
        sample.boxTemp = (uint16_t)(tempSensor.getTemperature() * 100);
        sample.humidity = (uint16_t)(tempSensor.getHumidity() * 100);
    } else {
        errorMsg = currentTime.toString(time_format_buf) + String(": Temperature sensor read error: ") + String(err);
        sd.logError(errorMsg);
        Serial.println(errorMsg);
    }

    err = pressureSensor.measure();
    if (err == NO_ERROR) {
        sample.waterHeight = (uint16_t)(pressureSensor.getDepth() * 100);
        sample.groundTemp = (uint16_t)(pressureSensor.getTemperature() * 100);
    } else {
        errorMsg = currentTime.toString(time_format_buf) + String(": Pressure sensor read error: ") + String(err);
        sd.logError(errorMsg);
        Serial.println(errorMsg);
    }

    err = distanceSensor.measure();
    if (err == NO_ERROR) {
        sample.peatHeight = (uint16_t)distanceSensor.getResult();
    } else {
        errorMsg = currentTime.toString(time_format_buf) + String(": Distance sensor read error: ") + String(err);
        sd.logError(errorMsg);
        Serial.println(errorMsg);
    }

    sample.time = (currentTime.unixtime() - SECONDS_FROM_1970_TO_2023);  // number of seconds since 01/01/2023

    // digitalWrite(PressureLidarVcc1, LOW);
    // digitalWrite(PressureLidarVcc2, LOW);
    // digitalWrite(TempVcc, LOW);

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

    sd.SetUp(id, Rtc.now().toString(time_format_buf));

    Serial.println("Paired!  Device ID: " + String(id));
    return id;
}

void setup() {
    // put your setup code here, to run once:
    Serial.begin(115200);

    pinMode(PressureLidarVcc1, OUTPUT);  // PressureLidarVcc1
    pinMode(PressureLidarVcc2, OUTPUT);  // PressureLidarVcc2
    pinMode(TempVcc, OUTPUT);            // TempVcc
    pinMode(SDVcc, OUTPUT);              // SDVcc

    digitalWrite(PressureLidarVcc1, HIGH);
    digitalWrite(PressureLidarVcc2, HIGH);
    digitalWrite(TempVcc, HIGH);
    digitalWrite(SDVcc, HIGH);

    pinMode(INTERRUPT_PIN, INPUT_PULLUP);
    attachInterrupt(INTERRUPT_PIN, alarmISR, FALLING);

    if (!Rtc.begin()) {
        Serial.println("Couldn't find RTC");
        return;
    }

    // Rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

    sd.Init();
    sd.DeleteFiles();
    espNow.init();

    currentTime = Rtc.now();

    deviceID = sd.getID();
    if (deviceID == 0) {
        deviceID = setupDevice();
        Error err = setupSensors(currentTime);
        espNow.sendStatusMessage(err, deviceID);
    } else {
        setupSensors(currentTime);
    }

    esp_sleep_enable_ext0_wakeup(GPIO_NUM_12, 0);
    setAlarmInterval(1);  // to wake the esp

    // digitalWrite(SDVcc, LOW);
    // esp_deep_sleep_start();
}

uint8_t buf[13] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int i = 0;
void loop() {
    measurement measure = takeSample(currentTime);
    //espNow.sendStatusMessage(0, deviceID);
    StructToArr(measure, buf);

    sd.saveMeasurement(i, buf);

    uint8_t buf2[13] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

    if (sd.getMeasurements(i, buf2, 1) == FATAL_ERROR) {
        Serial.println("Fatal error occured");
    }
    i++;
    measurement out = ArrToStruct(buf2);

    Serial.print("timeOut: ");
    DateTime timeOut(out.time + SECONDS_FROM_1970_TO_2023);
    char time_format_buf[] = "YYYY-MM-DD hh:mm:00";
    Serial.println(timeOut.toString(time_format_buf));
    Serial.print("peatHeight: ");
    Serial.println(out.peatHeight);
    Serial.print("waterHeight: ");
    Serial.println(out.waterHeight);
    Serial.print("boxTemp: ");
    Serial.println(out.boxTemp);
    Serial.print("groundTemp: ");
    Serial.println(out.groundTemp);
    Serial.print("humidity: ");
    Serial.println(out.humidity);

    delay(5000);
}