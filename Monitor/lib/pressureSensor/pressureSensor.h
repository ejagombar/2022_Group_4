#ifndef PRESSURESENSOR_H
#define PRESSURESENSOR_H

#include <Arduino.h>
#include <Wire.h>

#include <MS5837.h>
#include "sampleBuffer.h"
#include "Error.h"

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