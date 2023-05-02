#ifndef PRESSURESENSOR_H
#define PRESSURESENSOR_H

#include <Arduino.h>
#include <MS5837.h>
#include <Wire.h>

#include "Error.h"
#include "sampleBuffer.h"

class PressureSensor {
   private:
    MS5837 Sensor;
    int sampleSize;
    float pressureVariation, depthVariation, tempVariation;
    float fluidDensity;
    float depth;
    float temperature;
    float pressure;

   public:
    PressureSensor(float sampleSize_,float pressureVariation_, float depthVariation_, float tempVariation_, int fluidDensity_)
    :sampleSize(sampleSize_),
    pressureVariation(pressureVariation_),
    depthVariation(depthVariation_),
    tempVariation(tempVariation_),
    fluidDensity(fluidDensity_){};
    ~PressureSensor(){};
    Error setup();
    Error measure();
    float getDepth();
    float getTemperature();
    float getPressure();
};

#endif  // PRESSURESENSOR_H