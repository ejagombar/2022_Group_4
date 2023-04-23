#ifndef DISTANCESENSOR_H
#define DISTANCESENSOR_H

#include <Arduino.h>

#include "sampleBuffer.h"
#include "Error.h"

class DistanceSensor {
   private:
    const int SampleSize = 1;
    int result;

   public:
    DistanceSensor(){};
    ~DistanceSensor(){};
    Error setup();
    Error measure();
    int getResult();
};

#endif // DISTANCESENSOR_H