#include <Arduino.h>
#include <SPI.h>

#include "RTClib.h"

#define uS_TO_S_FACTOR 1000000ULL /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP 5           /* Time ESP32 will go to sleep (in seconds) */

RTC_DATA_ATTR int bootCount = 0;

#define INTERRUPT_PIN D10
RTC_PCF8523 rtc;

volatile bool alarm_triggered = false;

void hibernate() {
    // esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_SLOW_MEM, ESP_PD_OPTION_OFF);
    // esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_FAST_MEM, ESP_PD_OPTION_OFF);
    // esp_sleep_pd_config(ESP_PD_DOMAIN_XTAL, ESP_PD_OPTION_OFF);
    esp_deep_sleep_start();
}

void alarmISR() {
    alarm_triggered = true;
}

void setAlarmInterval(uint8_t interval) {
    alarm_triggered = false;
    DateTime current_time = rtc.now();

    uint8_t alarmMin = ceil(float(current_time.minute() + 1) / interval) * interval;
    if (alarmMin >= 60) {
        alarmMin = 0;
    }
    DateTime alarm_time(0, 0, 0, 0, alarmMin, 0);

    rtc.enableAlarm(alarm_time, PCF8523_AlarmMinute);

    char alarm_time_buf[] = "YYYY-MM-DDThh:mm:00";
    Serial.println(String("Alarm Time: ") + alarm_time.toString(alarm_time_buf));
}

void print_wakeup_reason() {
    esp_sleep_wakeup_cause_t wakeup_reason;

    wakeup_reason = esp_sleep_get_wakeup_cause();

    switch (wakeup_reason) {
        case ESP_SLEEP_WAKEUP_EXT0:
            Serial.println("Wakeup caused by external signal using RTC_IO");
            break;
        case ESP_SLEEP_WAKEUP_EXT1:
            Serial.println("Wakeup caused by external signal using RTC_CNTL");
            break;
        case ESP_SLEEP_WAKEUP_TIMER:
            Serial.println("Wakeup caused by timer");
            break;
        case ESP_SLEEP_WAKEUP_TOUCHPAD:
            Serial.println("Wakeup caused by touchpad");
            break;
        case ESP_SLEEP_WAKEUP_ULP:
            Serial.println("Wakeup caused by ULP program");
            break;
        default:
            Serial.printf("Wakeup was not caused by deep sleep: %d\n", wakeup_reason);
            break;
    }
}

void setup() {
    Serial.begin(115200);
    pinMode(INTERRUPT_PIN, INPUT_PULLUP);
    pinMode(D13, INPUT_PULLUP);
    attachInterrupt(INTERRUPT_PIN, alarmISR, FALLING);

    // Ensure RTC initializes
    if (!rtc.begin()) {
        Serial.println("Couldn't find RTC");
        return;
    }

    print_wakeup_reason();
    Serial.println("The time is " + rtc.now().timestamp());
    esp_sleep_enable_ext0_wakeup(GPIO_NUM_12, 0);

    setAlarmInterval(1);

    Serial.println("Going to sleep");
    delay(1000);
    hibernate();
    Serial.println("This shouldnt print");
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
