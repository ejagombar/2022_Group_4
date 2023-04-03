#ifndef DISTANCESENSOR_H
#define DISTANCESENSOR_H

#include <Arduino.h>
#include <Wire.h>

#include "Adafruit_VL53L0X.h"
#include "sampleBuffer.h"

typedef int8_t Error;

#define NO_ERROR 0
#define FATAL_ERROR -1
#define UNSTABLE_ERROR -2

class DistanceSensor {
   private:
    Adafruit_VL53L0X sensor;
    VL53L0X_RangingMeasurementData_t measureData;
    const int SampleSize = 5;
    int result;

   public:
    DistanceSensor(){};
    ~DistanceSensor(){};
    Error setup();
    Error measure();
    int getResult();
};

#endif // DISTANCESENSOR_H