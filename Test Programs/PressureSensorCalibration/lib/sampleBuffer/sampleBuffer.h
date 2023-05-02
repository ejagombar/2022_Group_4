#ifndef SAMPLEBUFFER_H
#define SAMPLEBUFFER_H

#include <Arduino.h>

class SampleBuffer {
   private:
    static const int MAX_SAMPLES = 50;
    float samples[MAX_SAMPLES];
    float maxVariation;
    int sampleCount;
    float average;

   public:
    SampleBuffer(float _maxVariation);
    ~SampleBuffer(){};

    void addSample(float sample);
    void calcAverage();
    float getAverage();
    bool isStable();
};

#endif // SAMPLEBUFFER_H