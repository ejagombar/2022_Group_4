#include "sampleBuffer.h"

SampleBuffer::SampleBuffer(float _maxVariation) {
    maxVariation = _maxVariation;
    sampleCount = 0;
}


void SampleBuffer::addSample(float sample) {
    if (sampleCount < MAX_SAMPLES) {
        samples[sampleCount] = sample;
        sampleCount++;
    }
}

void SampleBuffer::calcAverage() {
    float sum = 0;
    for (int i = 0; i < sampleCount; i++) {
        sum += samples[i];
    }
    average = sum / sampleCount;
}

float SampleBuffer::getAverage() {
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
