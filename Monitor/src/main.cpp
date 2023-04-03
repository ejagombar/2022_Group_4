#include <Arduino.h>
#include <RTClib.h>
#include <SPI.h>
#include <Wire.h>

#include "distanceSensor.h"
#include "pressureSensor.h"
#include "sampleBuffer.h"
#include "temperatureSensor.h"

DistanceSensor distanceSensor;
TemperatureSensor tempSensor;
PressureSensor pressureSensor;

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
char buf[16];
void loop() {
    uint32_t temp = Rtc.now().unixtime();
    temp = temp - SECONDS_FROM_1970_TO_2023;

    Serial.print("Actual Time: ");
    Serial.println(temp);

    temp = temp << 1;

    uint16_t height = 1238;
    uint16_t temperature = 9532;
    uint16_t depth = 2332;
    uint16_t temperature2 = 0xFFFF;
    uint16_t humidity = 0xAAAA;

    memcpy(buf, &temp, 3);
    memcpy(&buf[3], &height, 2);
    memcpy(&buf[5], &temperature, 2);
    memcpy(&buf[7], &depth, 2);
    memcpy(&buf[9], &temperature2, 2);
    memcpy(&buf[11], &humidity, 2);

    uint32_t timeOut = 0;
    uint16_t heightOut;
    uint16_t temperatureOut;
    uint16_t depthOut;
    uint16_t temperature2Out;
    uint16_t humidityOut;


    memcpy(&timeOut, &buf, 3);
    memcpy(&heightOut, &buf[3], 2);
    memcpy(&temperatureOut, &buf[5], 2);
    memcpy(&depthOut, &buf[7], 2);
    memcpy(&temperature2Out, &buf[9], 2);
    memcpy(&humidityOut, &buf[11], 2);

    Serial.print("timeOut: ");
    timeOut = timeOut >> 1;
    Serial.println(timeOut);
    Serial.print("heightOut: ");
    Serial.println(heightOut);
    Serial.print("temperatureOut: ");
    Serial.println(temperatureOut);
    Serial.print("depthOut: ");
    Serial.println(depthOut);
    Serial.print("temperature2Out: ");
    Serial.println(temperature2Out);
    Serial.print("humidityOut: ");
    Serial.println(humidityOut);
    // Serial.print("Time: ");
    // Serial.println(timeOut);
    
    // Serial.println(timeOut << 1);
    // Serial.println(timeOut >> 2);
    // Serial.println(buf);
    // Serial.println(Rtc.now().unixtime());
    delay(10000);
}