#include "temperatureSensor.h"

Error TemperatureSensor::setup() {
    Sensor.mSensorType = SHTSensor::SHTC3;
    if (Sensor.init()) {
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
        if (Sensor.readSample()) {
            TempSamples.addSample((int)(Sensor.getTemperature() * 100));
            HumiditySamples.addSample((int)(Sensor.getHumidity() * 100));
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