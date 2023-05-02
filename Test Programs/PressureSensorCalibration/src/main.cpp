#include <Arduino.h>

#include "pressureSensor.h"

#define PressureLidarVcc1 D11
#define PressureLidarVcc2 D12

PressureSensor pressureSensor;

void setup() {
    Serial.begin(115200);

    pinMode(PressureLidarVcc1, OUTPUT);
    pinMode(PressureLidarVcc2, OUTPUT);

    digitalWrite(PressureLidarVcc1, HIGH);
    digitalWrite(PressureLidarVcc2, HIGH);

    pressureSensor.setup();
}

void loop() {
    if (pressureSensor.measure() != NO_ERROR) {
        Serial.println("Error occured");
    }
    Serial.println(pressureSensor.getDepth());
}