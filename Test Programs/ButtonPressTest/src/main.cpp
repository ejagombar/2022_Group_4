#include <Arduino.h>

#include "button.h"

void printUp() { Serial.println("Up"); }

void printDown() { Serial.println("Down"); }

Button btnUp(D2, &printUp);
Button btnDown(D3, &printDown);
Button btnEnter(D5, &printDown);

void processButton(Button &btn) {
    if ((digitalRead(btn.getPin()) == LOW) && btn.getReady()) {
        btn.pressed();
        btn.callFunction();

        // Serial.println("Pressed");
    } else if ((digitalRead(btn.getPin()) == HIGH) &&
               (millis() - btn.getTimeOfPress() > btn.debounceTime)) {
        btn.setReady(true);
    }
}

void setup() {
    Serial.begin(115200);

    pinMode(btnUp.getPin(), INPUT_PULLUP);
    pinMode(btnDown.getPin(), INPUT_PULLUP);
    pinMode(btnEnter.getPin(), INPUT_PULLUP);
}

void loop() {
    processButton(btnUp);
    processButton(btnDown);
    processButton(btnEnter);
}