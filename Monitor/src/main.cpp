#include <Arduino.h>
#include <RTClib.h>
#include <SPI.h>
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

void setup() {
    // put your setup code here, to run once:
    Serial.begin(115200);

    pinMode(D12, OUTPUT);
    pinMode(D11, OUTPUT);
    pinMode(D10, OUTPUT);
    pinMode(D6, OUTPUT);

    digitalWrite(D12, HIGH);
    digitalWrite(D11, HIGH);
    digitalWrite(D10, HIGH);
    digitalWrite(D6, HIGH);

    pinMode(INTERRUPT_PIN, INPUT_PULLUP);
    attachInterrupt(INTERRUPT_PIN, alarmISR, FALLING);

    if (!Rtc.begin()) {
        Serial.println("Couldn't find RTC");
        return;
    }

    sd.Init();
    sd.clearFile();

    // esp_sleep_enable_ext0_wakeup(GPIO_NUM_12, 0);
    // setAlarmInterval(1);  // to wake the esp

    // if (distanceSensor.setup() != NO_ERROR) {
    //     Serial.println("Error occured in distance sensor setup");
    // };
    // if (tempSensor.setup() != NO_ERROR) {
    //     Serial.println("Error occured in temperature sensor setup");
    // };
    // if (pressureSensor.setup() != NO_ERROR) {
    //     Serial.println("Error occured in pressure sensor setup");
    // };

    // Error tmp = distanceSensor.measure();
    // if (tmp == NO_ERROR) {
    //     Serial.print("Distance: ");
    //     Serial.print(distanceSensor.getResult());
    //     Serial.print("mm\n");
    // } else {
    //     Serial.print("Distance sensor read error occured: ");
    //     Serial.println(tmp);
    // }

    // tmp = tempSensor.measure();
    // if (tmp == NO_ERROR) {
    //     Serial.print("Temperature: ");
    //     Serial.print(tempSensor.getTemperature());
    //     Serial.print("\n");
    // } else {
    //     Serial.print("Temperature sensor read error occured: ");
    //     Serial.println(tmp);
    // }

    // tmp = pressureSensor.measure();
    // if (tmp == NO_ERROR) {
    //     Serial.print("Depth: ");
    //     Serial.print(pressureSensor.getDepth());
    //     Serial.print("mm\n");
    //     Serial.print("Peat Temperature: ");
    //     Serial.print((float)pressureSensor.getTemperature() / 100);
    //     Serial.print("\n");
    // } else {
    //     Serial.print("Pressure sensor read error occured: ");
    //     Serial.println(tmp);
    // }

    // digitalWrite(D12, LOW);
    // digitalWrite(D10, LOW);
    // digitalWrite(D11, LOW);
    // digitalWrite(D6, LOW);

    // esp_deep_sleep_start();
}

void StructToArr2(measurement measurementIn, uint8_t* arrOut) {
    uint32_t tmp = measurementIn.time << 1;

    memcpy(arrOut, &tmp, 3);
    memcpy(&arrOut[3], &measurementIn.peatHeight, 2);
    memcpy(&arrOut[5], &measurementIn.waterHeight, 2);
    memcpy(&arrOut[7], &measurementIn.boxTemp, 2);
    memcpy(&arrOut[9], &measurementIn.groundTemp, 2);
    memcpy(&arrOut[11], &measurementIn.humidity, 2);
}

measurement ArrToStruct2(uint8_t* arrIn) {
    measurement measurementOut = {0, 0, 0, 0, 0, 0};
    uint32_t tmp;

    memcpy(&tmp, &arrIn, 3);
    measurementOut.time = tmp >> 1;

    memcpy(&measurementOut.peatHeight, &arrIn[3], 2);
    memcpy(&measurementOut.waterHeight, &arrIn[5], 2);
    memcpy(&measurementOut.boxTemp, &arrIn[7], 2);
    memcpy(&measurementOut.groundTemp, &arrIn[9], 2);
    memcpy(&measurementOut.humidity, &arrIn[11], 2);

    return measurementOut;
}

uint8_t buf[13] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int i = 0;
void loop() {
    // makeData();
    // readIt();
    // Serial.println("Done");

    // uint32_t temp = Rtc.now().unixtime();
    // temp = temp - SECONDS_FROM_1970_TO_2023;

    // Serial.print("Actual Time: ");
    // Serial.println(temp);

    // temp = temp << 1;

    // measurement measure = {temp, 100 * i, 128 * i, 2332, 0xFF * i, 0xAAAA};

    // StructToArr2(measure, buf);

    // sd.openFileWrite(DataFileOpen);
    // Serial.print("Measurement saved with code: ");
    uint8_t buf2[13] = {0xFF, 0, 0xFF, 0, 0xFF, 0xFF, 0xFF, 0, 0xFF, 0, 0xFF, 0, 0};
    sd.saveMeasurement(buf2);
    sd.getMeasurements(0, buf, 1);
    Serial.println(buf[0]);
    Serial.println(buf[1]);
    Serial.println(buf[2]);
    Serial.println(buf[3]);
    Serial.println(buf[4]);
    Serial.println(buf[5]);
    Serial.println(buf[6]);
    Serial.println(buf[7]);
    Serial.println(buf[8]);
    Serial.println(buf[9]);
    Serial.println(buf[10]);
    Serial.println(buf[11]);
    Serial.println(buf[12]);
    


    // uint8_t buf2[13] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

    // if (sd.getMeasurements(i, buf2, 1) == FATAL_ERROR) {
    //     Serial.println("Fatal error occured");
    // }
    // i++;
    // measurement out = ArrToStruct2(buf2);

    // Serial.print("timeOut: ");
    // Serial.println(out.time);
    // Serial.print("peatHeight: ");
    // Serial.println(out.peatHeight);
    // Serial.print("waterHeight: ");
    // Serial.println(out.waterHeight);
    // Serial.print("boxTemp: ");
    // Serial.println(out.boxTemp);
    // Serial.print("groundTemp: ");
    // Serial.println(out.groundTemp);
    // Serial.print("humidity: ");
    // Serial.println(out.humidity);

    delay(10000);
}