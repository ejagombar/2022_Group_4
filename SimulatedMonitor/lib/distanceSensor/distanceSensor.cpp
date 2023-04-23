#include "distanceSensor.h"

Error DistanceSensor::setup() {
    return NO_ERROR;
}

Error DistanceSensor::measure() {
    SampleBuffer mySamples(450);
    bool errorOccured = false;
    for (int i = 0; i < SampleSize; i++)  // take the sum of multiple readings
    {
        mySamples.addSample(random(20, 450));
    }
    if (!mySamples.isStable()) {
        return UNSTABLE_ERROR;
    }
    result = (int)mySamples.getAverage();
    return NO_ERROR;
}

int DistanceSensor::getResult() {
    return result;
}