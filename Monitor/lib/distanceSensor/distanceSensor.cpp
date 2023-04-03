#include "distanceSensor.h"

Error DistanceSensor::setup() {
    if (!sensor.begin()) {
        return FATAL_ERROR;
    }
    sensor.configSensor(Adafruit_VL53L0X::VL53L0X_SENSE_HIGH_ACCURACY);
    sensor.setMeasurementTimingBudgetMicroSeconds(200000);
    return NO_ERROR;
}

Error DistanceSensor::measure() {
    SampleBuffer mySamples(3);
    bool errorOccured = false;
    for (int i = 0; i < SampleSize; i++)  // take the sum of multiple readings
    {
        sensor.getSingleRangingMeasurement(&measureData, false);  // pass in 'true' to get debug data printout!

        if (measureData.RangeStatus == 0)  // keep only the good readings
        {
            mySamples.addSample(measureData.RangeMilliMeter);
        } else {
            errorOccured = true;
        }
    }
    if (errorOccured) {
        return FATAL_ERROR;
    }
    if (!mySamples.isStable()) {
        return UNSTABLE_ERROR;
    }
    result = mySamples.getAverage();
    return NO_ERROR;
}

int DistanceSensor::getResult() {
    return result;
}