#ifndef DISTANCESENSOR_H
#define DISTANCESENSOR_H

#include <Arduino.h>
#include <Wire.h>

#include "Adafruit_VL53L0X.h"
#include "sampleBuffer.h"
#include "Error.h"

class DistanceSensor {
   private:
    Adafruit_VL53L0X sensor;
    VL53L0X_RangingMeasurementData_t measureData;
    const int SampleSize = 50;
    float result;

   public:
    DistanceSensor(){};
    ~DistanceSensor(){};
    Error setup();
    Error measure();
    float getDistance();
};

#endif // DISTANCESENSOR_H