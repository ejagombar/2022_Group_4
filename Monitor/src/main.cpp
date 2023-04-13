#include <Arduino.h>
#include <RTClib.h>
#include <SPI.h>
#include <String.h>
#include <Wire.h>

#include "distanceSensor.h"
#include "pressureSensor.h"
#include "sampleBuffer.h"
#include "saved.h"
#include "temperatureSensor.h"

DistanceSensor distanceSensor;
TemperatureSensor tempSensor;
PressureSensor pressureSensor;
SDInterface sd;

#define PressureLidarVcc1 D11
#define PressureLidarVcc2 D12
#define TempVcc D10
#define SDVcc D6

#define SECONDS_FROM_1970_TO_2023 1672531200

#define INTERRUPT_PIN D13
RTC_PCF8523 Rtc;

volatile bool alarm_triggered = false;

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

measurement takeSample() {
    measurement sample;
    Error err;
    String errorMsg;
    DateTime currentTime = Rtc.now();
    char time_format_buf[] = "YYYY-MM-DDThh:mm:00";

    //=Setup Sensors================================================================================================


    //=Measure Sensors================================================================================================

    err = tempSensor.measure();
    if (err == NO_ERROR) {
        sample.boxTemp = (uint16_t)(tempSensor.getTemperature() * 100);
        sample.humidity = (uint16_t)(tempSensor.getHumidity() * 100);
    } else {
        errorMsg = currentTime.toString(time_format_buf) + String(": Temperature sensor read error: ") + String(err);
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

    err = pressureSensor.measure();
    if (err == NO_ERROR) {
        sample.waterHeight = (uint16_t)(pressureSensor.getDepth() * 100);
        sample.groundTemp = (uint16_t)(pressureSensor.getTemperature() * 100);
    } else {
        errorMsg = currentTime.toString(time_format_buf) + String(": Pressure sensor read error: ") + String(err);
        sd.logError(errorMsg);
        Serial.println(errorMsg);
    }

        Serial.print("timeOut: ");
    Serial.println(sample.time);
    Serial.print("peatHeight: ");
    Serial.println(sample.peatHeight);
    Serial.print("waterHeight: ");
    Serial.println(sample.waterHeight);
    Serial.print("boxTemp: ");
    Serial.println(sample.boxTemp);
    Serial.print("groundTemp: ");
    Serial.println(sample.groundTemp);
    Serial.print("humidity: ");
    Serial.println(sample.humidity);

    // digitalWrite(PressureLidarVcc1, LOW);
    // digitalWrite(PressureLidarVcc2, LOW);
    // digitalWrite(TempVcc, LOW);

    return sample;
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

    sd.Init();

    esp_sleep_enable_ext0_wakeup(GPIO_NUM_12, 0);
    setAlarmInterval(1);  // to wake the esp

    Error err;
    String errorMsg;
    DateTime currentTime = Rtc.now();
    char time_format_buf[] = "YYYY-MM-DDThh:mm:00";

    //=Setup Sensors================================================================================================

    err = distanceSensor.setup();
    if (err != NO_ERROR) {
        errorMsg = currentTime.toString(time_format_buf) + String(": Distance sensor setup error: ") + String(err);
        sd.logError(errorMsg);
        Serial.println(errorMsg);
    };

    err = tempSensor.setup();
    if (err != NO_ERROR) {
        errorMsg = currentTime.toString(time_format_buf) + String(": Temperature sensor setup error: ") + String(err);
        sd.logError(errorMsg);
        Serial.println(errorMsg);
    };

    err = pressureSensor.setup();
    if (err != NO_ERROR) {
        errorMsg = currentTime.toString(time_format_buf) + String(": Pressure sensor setup error: ") + String(err);
        sd.logError(errorMsg);
        Serial.println(errorMsg);
    };
    // digitalWrite(SDVcc, LOW);
    // esp_deep_sleep_start();
}

uint8_t buf[13] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int i = 0;  
void loop() {
    sd.logError("Looping..");
    uint32_t temp = Rtc.now().unixtime();
    temp = temp - SECONDS_FROM_1970_TO_2023;
    temp = temp / 60;  // minutes

    Serial.print("Actual Time: ");
    Serial.println(temp);

    measurement measure = takeSample();

    StructToArr(measure, buf);

    sd.saveMeasurement(buf);

    uint8_t buf2[13] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

    if (sd.getMeasurements(i, buf2, 1) == FATAL_ERROR) {
        Serial.println("Fatal error occured");
    }
    i++;
    measurement out = ArrToStruct(buf2);

    Serial.print("timeOut: ");
    Serial.println(out.time);
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