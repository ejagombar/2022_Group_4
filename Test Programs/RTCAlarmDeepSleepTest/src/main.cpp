#include <Arduino.h>
#include <SPI.h>

#include "RTClib.h"
#include <Arduino.h>

#include <Wire.h>

#include <SHTSensor.h>

#include <Adafruit_VL53L0X.h>

#include <MS5837.h>

#include <SD.h>


#define RTCMEMORYSTART 66
#define RTCMEMORYLEN 125
#define VCC_ADJ  1.0   // measure with your voltmeter and calculate that the number mesured from ESP is correct
MS5837 sensor;

SHTSensor sht;
// To use a specific sensor instead of probing the bus use this command:
// SHTSensor sht(SHTSensor::SHT3X);

File myFile;
const int chipSelect = 17;

Adafruit_VL53L0X lox = Adafruit_VL53L0X();
const int SampleSize = 25;  // define the sample size


#define uS_TO_S_FACTOR 1000000ULL /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP 5           /* Time ESP32 will go to sleep (in seconds) */

RTC_DATA_ATTR int bootCount = 0;

#define INTERRUPT_PIN D13
RTC_PCF8523 rtc;

volatile bool alarm_triggered = false;

VL53L0X_RangingMeasurementData_t measure;
float accumulator = 0;
float average;
void Lidar(){  
    
    
     for(int i = 0; i < SampleSize; i++)  // take the sum of multiple readings
        {
            lox.rangingTest(&measure, false); // pass in 'true' to get debug data printout!

            if (measure.RangeStatus != 4) // keep only the good readings
            {  
            accumulator += lox.readRange();
            }
        }
    average = accumulator / SampleSize;
    Serial.print("Height: ");
    Serial.print(average);  // print average of all samples
    Serial.print(" mm\n");
     

};

float humidity;
float temp;
void temperature(){
      if (sht.readSample()) {
      Serial.print("Humidity: ");
      humidity = sht.getHumidity();
      Serial.print(sht.getHumidity(), 2);
      Serial.print("\nTemperature (°C): ");
      temp = sht.getTemperature();
      Serial.print(sht.getTemperature(), 2);
      Serial.print("\n");
  } else {
      Serial.print("Error in readSample()\n");
  }
}

float press;
float altitude;
void pressure(){
    sensor.read();

    press = sensor.pressure();
    Serial.print("Pressure: "); 
    Serial.print(sensor.pressure()); 
    Serial.println(" mbar");

    altitude = sensor.altitude();
    Serial.print("Altitude: "); 
    Serial.print(sensor.altitude()); 
    Serial.println(" m above mean sea level\n");
}

void hibernate() {
    // esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_SLOW_MEM, ESP_PD_OPTION_OFF);
    // esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_FAST_MEM, ESP_PD_OPTION_OFF);
    // esp_sleep_pd_config(ESP_PD_DOMAIN_XTAL, ESP_PD_OPTION_OFF);
    esp_deep_sleep_start();
}

void alarmISR() {
    alarm_triggered = true;
}

void setAlarmInterval(uint8_t interval) {
    alarm_triggered = false;
     
    DateTime current_time = rtc.now();
   
    

    uint8_t alarmMin = ceil(float(current_time.minute() + 1) / interval) * interval;
    if (alarmMin >= 60) {
        alarmMin = 0;
    }
    DateTime alarm_time(current_time.year(), current_time.month(), current_time.day(), current_time.hour(), alarmMin, 0);

    rtc.enableAlarm(alarm_time, PCF8523_AlarmMinute);

    char alarm_time_buf[] = "YYYY-MM-DDThh:mm:00";
    Serial.println(String("Alarm Time: ") + alarm_time.toString(alarm_time_buf));
}

void print_wakeup_reason() {
    esp_sleep_wakeup_cause_t wakeup_reason;

    wakeup_reason = esp_sleep_get_wakeup_cause();

    switch (wakeup_reason) {
        case ESP_SLEEP_WAKEUP_EXT0:
            Serial.println("Wakeup caused by external signal using RTC_IO");
            break;
        case ESP_SLEEP_WAKEUP_EXT1:
            Serial.println("Wakeup caused by external signal using RTC_CNTL");
            break;
        case ESP_SLEEP_WAKEUP_TIMER:
            Serial.println("Wakeup caused by timer");
            break;
        case ESP_SLEEP_WAKEUP_TOUCHPAD:
            Serial.println("Wakeup caused by touchpad");
            break;
        case ESP_SLEEP_WAKEUP_ULP:
            Serial.println("Wakeup caused by ULP program");
            break;
        default:
            Serial.printf("Wakeup was not caused by deep sleep: %d\n", wakeup_reason);
            break;
    }
}

void setup() {
    Wire.begin();
    Serial.begin(115200);
    //Turn Digital Pin on to power sensors
    //SD card module
    pinMode(D6,OUTPUT);
    //Lidar
    pinMode(D5,OUTPUT);
    //temperature 
    pinMode(D3,OUTPUT);
    //pressure 
    pinMode(D12,OUTPUT);

    digitalWrite(D6,HIGH);
    digitalWrite(D5,HIGH);
    digitalWrite(D3,HIGH);
    digitalWrite(D12,HIGH);

    pinMode(INTERRUPT_PIN, INPUT_PULLUP);
    pinMode(D13, INPUT_PULLUP);
    attachInterrupt(INTERRUPT_PIN, alarmISR, FALLING);
    
    pinMode(SS, OUTPUT);

    // Ensure RTC initializes
    if (!rtc.begin()) {
        Serial.println("Couldn't find RTC");
        return;
    }
    
    // Ensure lidar is working 
    Serial.println("Adafruit VL53L0X test");
    if (!lox.begin()) {
        Serial.println(F("Failed to boot VL53L0X"));
        while(1);
    }
   
    Serial.println(F("VL53L0X API Simple Ranging example\n\n")); 

    //Ensure Temperature sensor is working 
    if (sht.init()) {
      Serial.print("SHTC3 boot : Success\n");
    } 
    else {
      Serial.print("SHTC3 boot : Failed\n");
    }
    sht.setAccuracy(SHTSensor::SHT_ACCURACY_MEDIUM); // only supported by SHT3x

    //Ensure Pressure sensor is working 
    while (!sensor.init()) {
    Serial.println("Init failed!");
    Serial.println("Are SDA/SCL connected correctly?");
    Serial.println("Blue Robotics Bar30: White=SDA, Green=SCL");
    Serial.println("\n\n\n");
  
    }
    sensor.setModel(MS5837::MS5837_30BA);
    sensor.setFluidDensity(997); // kg/m^3 (freshwater, 1029 for seawater)

    //Ensure SD card is working 
    if (!SD.begin(chipSelect)) {
    Serial.println("initialization failed!");
    return;
    }
    Serial.println("initialization done.");

    
    

    print_wakeup_reason();
    Serial.println("The time is " + rtc.now().timestamp());
    esp_sleep_enable_ext0_wakeup(GPIO_NUM_12, 0);
    //Turn On board LED on whilst board is not in deep sleep
    pinMode(D9,OUTPUT);
    digitalWrite(D9,HIGH);
    
    // open the file. note that only one file can be open at a time,
    // so you have to close this one before opening another.
    myFile = SD.open("/test.txt", FILE_WRITE);

    setAlarmInterval(1);//to wake the esp
    Lidar();
    temperature();
    pressure();
    Serial.println("Going to sleep");

    // if the file opened okay, write to it:
    if (myFile) {
        Serial.print("Writing to test.txt...");
        myFile.println("Time:" + rtc.now().timestamp());
        myFile.println("Height:");
        myFile.println(average);
        myFile.println("Humidity:");
        myFile.println(humidity);
        myFile.println("Temperature:");
        myFile.println(temp);
        myFile.println("Pressure:");
        myFile.println(press);
        myFile.println("Altitude:");
        myFile.println(altitude);

        // close the file:
        myFile.close();
        Serial.println("done.");
    } else {
        // if the file didn't open, print an error:
        Serial.println("error opening test.txt");
    }
    
    // re-open the file for reading:
    myFile = SD.open("/test.txt");
    if (myFile) {
        Serial.println("test.txt:");
        
        // read from the file until there's nothing else in it:
        while (myFile.available()) {
            Serial.write(myFile.read());
        }
        // close the file:
        myFile.close();
    } else {
        // if the file didn't open, print an error:
        Serial.println("error opening test.txt");
    }

    delay(4000);//wake for 4 sec

    digitalWrite(D6,LOW);
    digitalWrite(D5,LOW);
    digitalWrite(D3,LOW);
    digitalWrite(D2,LOW);
    hibernate();
    Serial.println("This shouldnt print");
}



void loop() {
 
    if (alarm_triggered) {
        Serial.println("Alarm triggered.\n");
        
       setAlarmInterval(1);
        // Set the RTC to the current time
        
        rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

     }else {
        Serial.println("Alarm not triggered. The time is " + rtc.now().timestamp());

        delay(1000);
    }
    
}




