#include "pressureSensor.h"

Error PressureSensor::setup() {
    return NO_ERROR;
}

Error PressureSensor::measure() {
    SampleBuffer depthSamples(30);
    SampleBuffer tempSample(30);
    bool errorOccured = false;
    for (int i = 0; i < SampleSize; i++)  // take the sum of multiple readings
    {

        depthSamples.addSample(random(0, 30));
        tempSample.addSample(random(5, 25));
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

float PressureSensor::getDepth() {
    return depth;
}

float PressureSensor::getTemperature() {
    return temperature;
}