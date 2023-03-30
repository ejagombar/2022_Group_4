#include <Arduino.h>

#include <Wire.h>

#include "SHTSensor.h"

#include "Adafruit_VL53L0X.h"

#include "MS5837.h"

MS5837 sensor;

SHTSensor sht;
// To use a specific sensor instead of probing the bus use this command:
// SHTSensor sht(SHTSensor::SHT3X);

Adafruit_VL53L0X lox = Adafruit_VL53L0X();

void setup() {
  // put your setup code here, to run once:

  Wire.begin();
  Serial.begin(115200);
  delay(1000); // let serial console settle

  if (sht.init()) {
      Serial.print("SHTC3 boot : Success\n");
  } 
  else {
      Serial.print("SHTC3 boot : Failed\n");
  }
  sht.setAccuracy(SHTSensor::SHT_ACCURACY_MEDIUM); // only supported by SHT3x

  Serial.println("Adafruit VL53L0X test");
  if (!lox.begin()) {
    Serial.println(F("Failed to boot VL53L0X"));
    while(1);
  }
  // power 
  Serial.println(F("VL53L0X API Simple Ranging example\n\n")); 

  while (!sensor.init()) {
    Serial.println("Init failed!");
    Serial.println("Are SDA/SCL connected correctly?");
    Serial.println("Blue Robotics Bar30: White=SDA, Green=SCL");
    Serial.println("\n\n\n");
    delay(5000);
  }

  sensor.setModel(MS5837::MS5837_30BA);
  sensor.setFluidDensity(997); // kg/m^3 (freshwater, 1029 for seawater)

}

const int SampleSize = 25;  // define the sample size

void loop() {
  // put your main code here, to run repeatedly:
  VL53L0X_RangingMeasurementData_t measure;
  float accumulator = 0;

  for(int i = 0; i < SampleSize; i++)  // take the sum of multiple readings
  {
    lox.rangingTest(&measure, false); // pass in 'true' to get debug data printout!

    if (measure.RangeStatus != 4) // keep only the good readings
    {  
      accumulator += lox.readRange();
    }
  }

  Serial.print("Height: ");
  Serial.print(accumulator / SampleSize);  // print average of all samples
  Serial.print(" mm\n");

  if (sht.readSample()) {
      Serial.print("Humidity: ");
      Serial.print(sht.getHumidity(), 2);
      Serial.print("\nTemperature (°C): ");
      Serial.print(sht.getTemperature(), 2);
      Serial.print("\n");
  } else {
      Serial.print("Error in readSample()\n");
  }

  sensor.read();

  Serial.print("Pressure: "); 
  Serial.print(sensor.pressure()); 
  Serial.println(" mbar");

  Serial.print("Altitude: "); 
  Serial.print(sensor.altitude()); 
  Serial.println(" m above mean sea level\n");


  delay(50);

}