#include "pressureSensor.h"

Error PressureSensor::setup() {
    if (!Sensor.init()) {
        return FATAL_ERROR;
    }
    Sensor.setModel(MS5837::MS5837_02BA);
    Sensor.setFluidDensity(fluidDensity);  // kg/m^3 (freshwater, 1029 for seawater)
    return NO_ERROR;
}

Error PressureSensor::measure() {
    SampleBuffer depthSamples(depthVariation);
    SampleBuffer tempSample(tempVariation);
    SampleBuffer pressureSample(pressureVariation);
    bool errorOccured = false;
    for (int i = 0; i < sampleSize; i++)  // take the sum of multiple readings
    {
        Sensor.read();
        depthSamples.addSample(Sensor.depth());
        tempSample.addSample(Sensor.temperature());
        pressureSample.addSample(Sensor.pressure());
    }

    if (!depthSamples.isStable()) {
        return UNSTABLE_ERROR;
    }

    if (!tempSample.isStable()) {
        return UNSTABLE_ERROR;
    }

    if (!pressureSample.isStable()) {
        return UNSTABLE_ERROR;
    }

    depth = depthSamples.getAverage();
    temperature = tempSample.getAverage();
    pressure = pressureSample.getAverage();
    return NO_ERROR;
}

float PressureSensor::getDepth() {
    return depth;
}

float PressureSensor::getTemperature() {
    return temperature;
}

float PressureSensor::getPressure() {
    return pressure;
}