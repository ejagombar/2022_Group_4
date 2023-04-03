#include <Arduino.h>
#include <Wire.h>

#include "Adafruit_VL53L0X.h"

class Samples {
   private:
    static const int MAX_SAMPLES = 10;
    int samples[MAX_SAMPLES];
    int maxVariation;
    int sampleCount;
    int average;

   public:
    Samples(int _maxVariation) {
        maxVariation = _maxVariation;
        sampleCount = 0;
    }

    ~Samples(){};

    void addSample(int sample) {
        if (sampleCount < MAX_SAMPLES) {
            samples[sampleCount] = sample;
            sampleCount++;
        }
    }

    void calcAverage() {
        int sum = 0;
        for (int i = 0; i < sampleCount; i++) {
            sum += samples[i];
        }
        average = sum / sampleCount;
    }

    int getAverage() {
        return average;
    }

    bool isStable() {
        calcAverage();
        for (int i = 0; i < sampleCount; i++) {
            if (abs(samples[i] - average) > maxVariation) {
                return false;
            }
        }
        return true;
    }
};

typedef int8_t Error;

#define NO_ERROR 0
#define FATAL_ERROR -1
#define UNSTABLE_ERROR -2

class DistanceSensor {
   private:
    Adafruit_VL53L0X sensor;
    VL53L0X_RangingMeasurementData_t measureData;
    const int SampleSize = 5;
    int result;

   public:
    DistanceSensor(){};
    ~DistanceSensor(){};
    Error setup();
    Error measure();
    int getResult();
};

Error DistanceSensor::setup() {
    Serial.println("Adafruit VL53L0X test");
    if (!sensor.begin()) {
        Serial.println(F("Failed to boot VL53L0X"));
        return FATAL_ERROR;
    }
    sensor.configSensor(Adafruit_VL53L0X::VL53L0X_SENSE_HIGH_ACCURACY);
    sensor.setMeasurementTimingBudgetMicroSeconds(200000);
    return NO_ERROR;
}

Error DistanceSensor::measure() {
    Samples mySamples(3);
    bool errorOccured = false;
    for (int i = 0; i < SampleSize; i++)  // take the sum of multiple readings
    {
        sensor.getSingleRangingMeasurement(&measureData, false);  // pass in 'true' to get debug data printout!

        if (measureData.RangeStatus == 0)  // keep only the good readings
        {
            mySamples.addSample(measureData.RangeMilliMeter);
        } else {
            errorOccured = true;
            Serial.println("Error with range status");
        }
    }
    if (errorOccured) {
        return FATAL_ERROR;
    }
    if (!mySamples.isStable()) {
        return UNSTABLE_ERROR;
    }
    result = mySamples.getAverage();
    return NO_ERROR;
}

int DistanceSensor::getResult() {
    return result;
}
DistanceSensor DistSensor;

void setup() {
    // put your setup code here, to run once:

    pinMode(D12, OUTPUT);
    digitalWrite(D12, HIGH);

    pinMode(D11, OUTPUT);
    digitalWrite(D11, HIGH);

    Serial.begin(115200);

    if (DistSensor.setup() != NO_ERROR) {
        Serial.println("Error occured in setup");
    };
    delay(1000);
}

void loop() {
    // put your main code here, to run repeatedly:
    Error tmp = DistSensor.measure();
    if (tmp == NO_ERROR) {
        Serial.print("Distance: ");
        Serial.print(DistSensor.getResult());
        Serial.print("mm\n");
    } else {
        Serial.print("Error occured: ");
        Serial.println(tmp);
    }
    // delay(1000);
}