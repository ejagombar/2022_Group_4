#include "temperatureSensor.h"

Error TemperatureSensor::setup() {
    return NO_ERROR;
}

Error TemperatureSensor::measure() {
    SampleBuffer TempSamples(30);
    SampleBuffer HumiditySamples(100);
    bool errorOccured = false;
    for (int i = 0; i < SampleSize; i++)  // take the sum of multiple readings
    {
        TempSamples.addSample(random(5, 25));
        HumiditySamples.addSample(random(1, 100));
    }
    if (errorOccured) {
        return FATAL_ERROR;
    }
    if (!TempSamples.isStable()) {
        return UNSTABLE_ERROR;
    }
    if (!HumiditySamples.isStable()) {
        return UNSTABLE_ERROR;
    }
    temp = TempSamples.getAverage();
    humidity = HumiditySamples.getAverage();
    return NO_ERROR;
}

float TemperatureSensor::getTemperature() {
    return temp;
}

float TemperatureSensor::getHumidity() {
    return humidity;
}