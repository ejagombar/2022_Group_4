#include <Arduino.h>
#include "RTClib.h"
#include <SPI.h>

// Change this to reflect the interrupt pin you would like to use
#define INTERRUPT_PIN D10
RTC_PCF8523 rtc;

volatile bool alarm_triggered = false;



void alarmISR() {
  alarm_triggered = true;
}


void doAlarmExample() {
  // Make alarm trigger one minute after current time, but not accounting for
  // seconds as the PCF8523 alarm doesn't have an alarm seconds register. So for
  // example if current_time is 12:00:50, and alarm_time is 12:01:50, it will
  // trigger at the exact minute, e.g. 12:01:00, because the PCF8523 hardware
  // does not allow for seconds to be set on the alarm and ignores them.
  DateTime current_time = rtc.now();
  TimeSpan alarm_offset(0, 0, 1, 0); // One minute
  DateTime alarm_time(current_time + alarm_offset);

  // Good to clear other timers and such.
  rtc.deconfigureAllTimers();

  rtc.enableAlarm(alarm_time, PCF8523_AlarmDate);

  // Print the current time in ISO8601 format.
  Serial.println(String("Current Time: ") + current_time.timestamp());
  Serial.flush();

  // Print when the alarm should trigger. Using a custom ISO8601 format
  // because as stated above, PCF8523 can only trigger on the :00th second
  // so seconds don't matter.
  char alarm_time_buf[] = "YYYY-MM-DDThh:mm:00";
  Serial.println(String("Alarm Time: ") + alarm_time.toString(alarm_time_buf));
  Serial.flush();

}



void setup() {
  Serial.begin(115200);

  	pinMode(INTERRUPT_PIN, INPUT_PULLUP);
	  attachInterrupt(INTERRUPT_PIN, alarmISR, FALLING);

  // Ensure RTC initializes
  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    while (1)
      delay(10);
  }

  doAlarmExample();
  alarm_triggered = false;
}

void loop() {
  if (alarm_triggered) {
    alarm_triggered = false;
    rtc.disableAlarm();
    Serial.println("Alarm triggered.\n");
    doAlarmExample();
    alarm_triggered = false;
  } else {
    Serial.println("Alarm not triggered. The time is " + rtc.now().timestamp());
    delay(1000);
  }
}



// struct Button {
// 	const uint8_t PIN;
// 	uint32_t numberKeyPresses;
// 	bool pressed;
// };

// Button button1 = {D10, 0, false};

// void IRAM_ATTR isr() {
// 	button1.numberKeyPresses++;
// 	button1.pressed = true;
// }

// void setup() {
// 	Serial.begin(115200);

// }

// void loop() {
// 	if (button1.pressed) {
// 		Serial.printf("Button has been pressed %u times\n", button1.numberKeyPresses);
// 		button1.pressed = false;
// 	}
// }