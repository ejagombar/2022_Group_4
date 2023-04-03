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
    esp_sleep_enable_ext0_wakeup(GPIO_NUM_12, 0);
    setAlarmInterval(1);  // to wake the esp

    if (distanceSensor.setup() != NO_ERROR) {
        Serial.println("Error occured in distance sensor setup");
    };
    if (tempSensor.setup() != NO_ERROR) {
        Serial.println("Error occured in temperature sensor setup");
    };
    if (pressureSensor.setup() != NO_ERROR) {
        Serial.println("Error occured in pressure sensor setup");
    };

    Error tmp = distanceSensor.measure();
    if (tmp == NO_ERROR) {
        Serial.print("Distance: ");
        Serial.print(distanceSensor.getResult());
        Serial.print("mm\n");
    } else {
        Serial.print("Distance sensor read error occured: ");
        Serial.println(tmp);
    }

    tmp = tempSensor.measure();
    if (tmp == NO_ERROR) {
        Serial.print("Temperature: ");
        Serial.print(tempSensor.getTemperature());
        Serial.print("\n");
    } else {
        Serial.print("Temperature sensor read error occured: ");
        Serial.println(tmp);
    }

    tmp = pressureSensor.measure();
    if (tmp == NO_ERROR) {
        Serial.print("Depth: ");
        Serial.print(pressureSensor.getDepth());
        Serial.print("mm\n");
        Serial.print("Peat Temperature: ");
        Serial.print((float)pressureSensor.getTemperature() / 100);
        Serial.print("\n");
    } else {
        Serial.print("Pressure sensor read error occured: ");
        Serial.println(tmp);
    }

    digitalWrite(D12, LOW);
    digitalWrite(D10, LOW);
    digitalWrite(D11, LOW);
    digitalWrite(D6, LOW);

    esp_deep_sleep_start();
}

void loop() {
    Serial.println("HOW DID YOU GET HERE?");

    delay(1000);
}