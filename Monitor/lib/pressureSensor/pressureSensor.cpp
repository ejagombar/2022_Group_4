#include "pressureSensor.h"

Error PressureSensor::setup() {
    if (!Sensor.init()) {
        Serial.println(F("Failed to boot Pressure Sensor"));
        return FATAL_ERROR;
    }
    Sensor.setModel(MS5837::MS5837_02BA);
    Sensor.setFluidDensity(997);  // kg/m^3 (freshwater, 1029 for seawater)
    return NO_ERROR;
}

Error PressureSensor::measure() {
    SampleBuffer depthSamples(3);
    SampleBuffer tempSample(3);
    bool errorOccured = false;
    for (int i = 0; i < SampleSize; i++)  // take the sum of multiple readings
    {
        Sensor.read();
        depthSamples.addSample((int)(Sensor.depth() * 1000));
        tempSample.addSample((int)(Sensor.temperature() * 100));
    }

    if (!depthSamples.isStable()) {
        return UNSTABLE_ERROR;
    }

    if (!tempSample.isStable()) {
        return UNSTABLE_ERROR;
    }

    depth = depthSamples.getAverage();
    temperature = tempSample.getAverage();
    return NO_ERROR;
}

int PressureSensor::getDepth() {
    return depth;
}

int PressureSensor::getTemperature() {
    return temperature;
}