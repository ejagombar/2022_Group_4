#include <Arduino.h>

#include "pressureSensor.h"

#define PressureLidarVcc1 D11
#define PressureLidarVcc2 D12

PressureSensor pressureSensor(20, 5, 5, 5, 997);

void setup() {
    Serial.begin(115200);

    pinMode(PressureLidarVcc1, OUTPUT);
    pinMode(PressureLidarVcc2, OUTPUT);

    digitalWrite(PressureLidarVcc1, HIGH);
    digitalWrite(PressureLidarVcc2, HIGH);

    Wire.begin(SDA, SCL);

    if (pressureSensor.setup() != NO_ERROR) {
        Serial.println("Error occured on setupt");
    }
}

void loop() {
    if (pressureSensor.measure() != NO_ERROR) {
        Serial.println("Error occured");
    }
    Serial.print("Depth (m):");
    Serial.println(pressureSensor.getDepth());
    Serial.print("Pressure (mbar):");
    Serial.println(pressureSensor.getPressure());
    Serial.print("Temp (c):");
    Serial.println(pressureSensor.getTemperature());
    Serial.println();
}