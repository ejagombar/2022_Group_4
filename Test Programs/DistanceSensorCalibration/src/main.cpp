#include <Arduino.h>

#include "distanceSensor.h"

#define PressureLidarVcc1 D11
#define PressureLidarVcc2 D12

DistanceSensor distanceSensor;

void setup() {
    Serial.begin(115200);

    pinMode(PressureLidarVcc1, OUTPUT);
    pinMode(PressureLidarVcc2, OUTPUT);

    digitalWrite(PressureLidarVcc1, HIGH);
    digitalWrite(PressureLidarVcc2, HIGH);

    distanceSensor.setup();
}

void loop() {
    if (distanceSensor.measure() != NO_ERROR) {
        Serial.println("Error occured");
    }
    Serial.println(distanceSensor.getDistance());
}