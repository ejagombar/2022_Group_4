#ifndef PRESSURESENSOR_H
#define PRESSURESENSOR_H

#include <Arduino.h>
#include <Wire.h>

#include <MS5837.h>
#include "sampleBuffer.h"

typedef int8_t Error;

#define NO_ERROR 0
#define FATAL_ERROR -1
#define UNSTABLE_ERROR -2

class PressureSensor {
   private:
    MS5837 Sensor;
    const int SampleSize = 5;
    int depth;
    int temperature;

   public:
    PressureSensor(){};
    ~PressureSensor(){};
    Error setup();
    Error measure();
    int getDepth();
    int getTemperature();
};

#endif // PRESSURESENSOR_H