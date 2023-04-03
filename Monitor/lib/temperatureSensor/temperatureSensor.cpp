#include "temperatureSensor.h"

Error TemperatureSensor::setup() {
    TempSensor.mSensorType = SHTSensor::SHTC3;
    if (TempSensor.init()) {
        return NO_ERROR;
    } else {
        return FATAL_ERROR;
    }
}

Error TemperatureSensor::measure() {
    SampleBuffer TempSamples(3);
    SampleBuffer HumiditySamples(3);
    bool errorOccured = false;
    for (int i = 0; i < SampleSize; i++)  // take the sum of multiple readings
    {
        if (TempSensor.readSample()) {
            TempSamples.addSample((int)(TempSensor.getTemperature() * 100));
            HumiditySamples.addSample((int)(TempSensor.getHumidity() * 100));
        } else {
            errorOccured = true;
        }
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
    temp = TempSamples.getAverage() / 100.0;
    humidity = HumiditySamples.getAverage() / 100.0;
    return NO_ERROR;
}

float TemperatureSensor::getTemperature() {
    return temp;
}

float TemperatureSensor::getHumidity() {
    return humidity;
}