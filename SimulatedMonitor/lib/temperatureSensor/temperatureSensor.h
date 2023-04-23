#ifndef TEMPERATURESENSOR_H
#define TEMPERATURESENSOR_H

#include <Arduino.h>
#include <SHTSensor.h>
#include <Wire.h>

#include "sampleBuffer.h"
#include "Error.h"

class TemperatureSensor {
   private:
    SHTSensor Sensor;
    const int SampleSize = 5;
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