#include <Arduino.h>

#include <Wire.h>

#include "SHTSensor.h"



SHTSensor TempSensor(SHTSensor::SHTC3);

void setup() {
  // put your setup code here, to run once:

  pinMode(D10, OUTPUT);
  digitalWrite(D10, HIGH);

  Wire.begin();
  Serial.begin(115200);
  delay(1000); // let serial console settle

  if (TempSensor.init()) {
      Serial.print("SHTC3 boot : Success\n");
  } 
  else {
      Serial.print("SHTC3 boot : Failed\n");
  }

}

void loop() {

  if (TempSensor.readSample()) {
      Serial.print("Humidity: ");
      Serial.print(TempSensor.getHumidity(), 2);
      Serial.print("\nTemperature (°C): ");
      Serial.print(TempSensor.getTemperature(), 2);
      Serial.print("\n");
  } else {
      Serial.print("Error in readSample()\n");
  }

  delay(1000);

}