#include <Arduino.h>
#include "distanceSensor.h"
#include "temperatureSensor.h"
#include "sampleBuffer.h"


DistanceSensor DistSensor;

void setup() {
    // put your setup code here, to run once:

    pinMode(D12, OUTPUT);
    digitalWrite(D12, HIGH);

    pinMode(D11, OUTPUT);
    digitalWrite(D11, HIGH);

    Serial.begin(115200);

    if (DistSensor.setup() != NO_ERROR) {
        Serial.println("Error occured in setup");
    };
    delay(1000);
}

void loop() {
    // put your main code here, to run repeatedly:
    Error tmp = DistSensor.measure();
    if (tmp == NO_ERROR) {
        Serial.print("Distance: ");
        Serial.print(DistSensor.getResult());
        Serial.print("mm\n");
    } else {
        Serial.print("Error occured: ");
        Serial.println(tmp);
    }
    // delay(1000);
}