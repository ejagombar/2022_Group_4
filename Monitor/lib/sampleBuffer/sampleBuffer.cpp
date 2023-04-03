#include "sampleBuffer.h"

SampleBuffer::SampleBuffer(int _maxVariation) {
    maxVariation = _maxVariation;
    sampleCount = 0;
}

SampleBuffer::~SampleBuffer(){};

void SampleBuffer::addSample(int sample) {
    if (sampleCount < MAX_SAMPLES) {
        samples[sampleCount] = sample;
        sampleCount++;
    }
}

void SampleBuffer::calcAverage() {
    int sum = 0;
    for (int i = 0; i < sampleCount; i++) {
        sum += samples[i];
    }
    average = sum / sampleCount;
}

int SampleBuffer::getAverage() {
    return average;
}

bool SampleBuffer::isStable() {
    calcAverage();
    for (int i = 0; i < sampleCount; i++) {
        if (abs(samples[i] - average) > maxVariation) {
            return false;
        }
    }
    return true;
}
