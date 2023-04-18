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
    SampleBuffer TempSamples(1.0);
    SampleBuffer HumiditySamples(1.0);
    bool errorOccured = false;
    for (int i = 0; i < SampleSize; i++)  // take the sum of multiple readings
    {
        if (Sensor.readSample()) {
            TempSamples.addSample(Sensor.getTemperature());
            HumiditySamples.addSample(Sensor.getHumidity());
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