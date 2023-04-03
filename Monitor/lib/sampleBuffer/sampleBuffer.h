#ifndef SAMPLEBUFFER_H
#define SAMPLEBUFFER_H

#include <Arduino.h>

class SampleBuffer {
   private:
    static const int MAX_SAMPLES = 10;
    int samples[MAX_SAMPLES];
    int maxVariation;
    int sampleCount;
    int average;

   public:
    SampleBuffer(int _maxVariation);
    ~SampleBuffer(){};

    void addSample(int sample);
    void calcAverage();
    int getAverage();
    bool isStable();
};

#endif // SAMPLEBUFFER_H