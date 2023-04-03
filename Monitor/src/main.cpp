#include <Arduino.h>

#include "distanceSensor.h"
#include "pressureSensor.h"
#include "sampleBuffer.h"
#include "temperatureSensor.h"

DistanceSensor distanceSensor;
TemperatureSensor tempSensor;
PressureSensor pressureSensor;

void setup() {
    // put your setup code here, to run once:

    pinMode(D12, OUTPUT);
    digitalWrite(D12, HIGH);

    pinMode(D11, OUTPUT);
    digitalWrite(D11, HIGH);

    pinMode(D10, OUTPUT);
    digitalWrite(D10, HIGH);

    Serial.begin(115200);

    if (distanceSensor.setup() != NO_ERROR) {
        Serial.println("Error occured in distance sensor setup");
    };
    if (tempSensor.setup() != NO_ERROR) {
        Serial.println("Error occured in temperature sensor setup");
    };
    if (pressureSensor.setup() != NO_ERROR) {
        Serial.println("Error occured in pressure sensor setup");
    };
    delay(1000);
}

void loop() {
    // put your main code here, to run repeatedly:
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
        Serial.print((float)pressureSensor.getTemperature()/100);
        Serial.print("\n");
    } else {
        Serial.print("Pressure sensor read error occured: ");
        Serial.println(tmp);
    }
    delay(1000);
}