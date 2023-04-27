#ifndef TEMPERATURESENSOR_H
#define TEMPERATURESENSOR_H

#include <Arduino.h>

#include "sampleBuffer.h"
#include "Error.h"

class TemperatureSensor {
   private:
    const int SampleSize = 1;
    float temp;
    float humidity;

   public:
    TemperatureSensor() {};
    ~TemperatureSensor() {};
    Error setup();
    Error measure();
    float getTemperature();
    float getHumidity();
};

#endif  // TEMPERATURESENSOR_H