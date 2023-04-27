#ifndef PRESSURESENSOR_H
#define PRESSURESENSOR_H

#include <Arduino.h>

#include "sampleBuffer.h"
#include "Error.h"

class PressureSensor {
   private:
    const int SampleSize = 1;
    float depth;
    float temperature;

   public:
    PressureSensor(){};
    ~PressureSensor(){};
    Error setup();
    Error measure();
    float getDepth();
    float getTemperature();
};

#endif // PRESSURESENSOR_H