#include "pressureSensor.h"

Error PressureSensor::setup() {
    if (!Sensor.init()) {
        return FATAL_ERROR;
    }
    Sensor.setModel(MS5837::MS5837_02BA);
    Sensor.setFluidDensity(997);  // kg/m^3 (freshwater, 1029 for seawater)
    return NO_ERROR;
}

Error PressureSensor::measure() {
    SampleBuffer depthSamples(50);
    SampleBuffer tempSample(50);
    bool errorOccured = false;
    for (int i = 0; i < SampleSize; i++)  // take the sum of multiple readings
    {
        Sensor.read();
        depthSamples.addSample(Sensor.depth());
        tempSample.addSample(Sensor.temperature());
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