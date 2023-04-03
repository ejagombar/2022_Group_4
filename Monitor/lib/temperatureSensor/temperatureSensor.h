#ifndef TEMPERATURESENSOR_H
#define TEMPERATURESENSOR_H

#include <Arduino.h>
#include <SHTSensor.h>
#include <Wire.h>

#include "sampleBuffer.h"

typedef int8_t Error;

#define NO_ERROR 0
#define FATAL_ERROR -1
#define UNSTABLE_ERROR -2

class TemperatureSensor {
   private:
    SHTSensor Sensor;
    const int SampleSize = 5;
    float temp;
    float humidity;

   public:
    TemperatureSensor();
    Error setup();
    Error measure();
    float getTemperature();
    float getHumidity();
};

#endif  // TEMPERATURESENSOR_H