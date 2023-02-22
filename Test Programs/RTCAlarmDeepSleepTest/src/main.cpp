#include <Arduino.h>
#include <SPI.h>

#include "RTClib.h"

#define INTERRUPT_PIN D10
RTC_PCF8523 rtc;

volatile bool alarm_triggered = false;

void alarmISR() {
    alarm_triggered = true;
}

void setAlarmInterval(uint8_t interval) {
    alarm_triggered = false;
    DateTime current_time = rtc.now();

    uint8_t alarmMin = ceil(float(current_time.minute()+1) / interval) * interval;
    if (alarmMin >= 60) {
        alarmMin = 0;
    }
    DateTime alarm_time(0, 0, 0, 0, alarmMin, 0);

    rtc.enableAlarm(alarm_time, PCF8523_AlarmMinute);

    char alarm_time_buf[] = "YYYY-MM-DDThh:mm:00";
    Serial.println(String("Alarm Time: ") + alarm_time.toString(alarm_time_buf));
}

void setup() {
    Serial.begin(115200);

    pinMode(INTERRUPT_PIN, INPUT_PULLUP);
    attachInterrupt(INTERRUPT_PIN, alarmISR, FALLING);

    // Ensure RTC initializes
    if (!rtc.begin()) {
        Serial.println("Couldn't find RTC");
        return;
    }

    setAlarmInterval(1);
}

void loop() {
    if (alarm_triggered) {
        Serial.println("Alarm triggered.\n");
        setAlarmInterval(1);
    } else {
        Serial.println("Alarm not triggered. The time is " + rtc.now().timestamp());

        delay(1000);
    }
}
