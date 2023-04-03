
#include <Adafruit_VL53L0X.h>
#include <Arduino.h>
#include <MS5837.h>
#include <RTClib.h>
#include <SD.h>
#include <SHTSensor.h>
#include <SPI.h>
#include <Wire.h>

MS5837 PressureSensor;

SHTSensor TempSensor(SHTSensor::SHTC3);

Adafruit_VL53L0X DistanceSensor = Adafruit_VL53L0X();

#define INTERRUPT_PIN D13
RTC_PCF8523 Rtc;

File myFile;
const int SDCardCS = D9;

const int SampleSize = 25;  // define the sample size

volatile bool alarm_triggered = false;

float average;
float humidity;
float temp;
float press;
float altitude;

void Lidar() {
    VL53L0X_RangingMeasurementData_t DistanceMeasure;
    float accumulator = 0;

    for (int i = 0; i < SampleSize; i++)  // take the sum of multiple readings
    {
        DistanceSensor.rangingTest(&DistanceMeasure, false);  // pass in 'true' to get debug data printout!

        if (DistanceMeasure.RangeStatus != 4)  // keep only the good readings
        {
            accumulator += DistanceSensor.readRange();
        }
    }
    average = accumulator / SampleSize;
    Serial.print("Height: ");
    Serial.print(average);  // print average of all samples
    Serial.print(" mm\n");
};

void temperature() {
    if (TempSensor.readSample()) {
        Serial.print("Humidity: ");
        humidity = TempSensor.getHumidity();
        Serial.print(TempSensor.getHumidity(), 2);
        Serial.print("\nTemperature (°C): ");
        temp = TempSensor.getTemperature();
        Serial.print(TempSensor.getTemperature(), 2);
        Serial.print("\n");
    } else {
        Serial.print("Error in readSample()\n");
    }
}

void pressure() {
    PressureSensor.read();

    press = PressureSensor.pressure();
    Serial.print("Pressure: ");
    Serial.print(PressureSensor.pressure());
    Serial.println(" mbar");

    altitude = PressureSensor.altitude();
    Serial.print("Altitude: ");
    Serial.print(PressureSensor.altitude());
    Serial.println(" m above mean sea level\n");
}

void alarmISR() {
    alarm_triggered = true;
}

void setAlarmInterval(uint8_t interval) {
    alarm_triggered = false;

    DateTime current_time = Rtc.now();

    uint8_t alarmMin = ceil(float(current_time.minute() + 1) / interval) * interval;
    if (alarmMin >= 60) {
        alarmMin = 0;
    }
    DateTime alarm_time(current_time.year(), current_time.month(), current_time.day(), current_time.hour(), alarmMin, 0);

    Rtc.enableAlarm(alarm_time, PCF8523_AlarmMinute);

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
    delay(1000);
    // Turn Digital Pin on to power sensors
    pinMode(D12, OUTPUT);
    digitalWrite(D12, HIGH);
    pinMode(D10, OUTPUT);
    digitalWrite(D10, HIGH);
    pinMode(D11, OUTPUT);
    digitalWrite(D11, HIGH);

    pinMode(D6, OUTPUT);
    digitalWrite(D6, HIGH);

    pinMode(INTERRUPT_PIN, INPUT_PULLUP);
    attachInterrupt(INTERRUPT_PIN, alarmISR, FALLING);

    pinMode(SS, OUTPUT);

    if (!Rtc.begin()) {
        Serial.println("Couldn't find RTC");
        return;
    }
    esp_sleep_enable_ext0_wakeup(GPIO_NUM_12, 0);
    setAlarmInterval(60);  // to wake the esp

    Serial.println("Adafruit VL53L0X test");
    if (!DistanceSensor.begin()) {
        Serial.println(F("Failed to boot VL53L0X"));
        while (1)
            ;
    }

    if (TempSensor.init()) {
        Serial.print("SHTC3 boot : Success\n");
    } else {
        Serial.print("SHTC3 boot : Failed\n");
    }

    while (!PressureSensor.init()) {
        Serial.println("Init failed!");
        Serial.println("Are SDA/SCL connected correctly?");
        Serial.println("Blue Robotics Bar30: White=SDA, Green=SCL");
        Serial.println("\n\n\n");
    }
    PressureSensor.setModel(MS5837::MS5837_02BA);
    PressureSensor.setFluidDensity(997);  // kg/m^3 (freshwater, 1029 for seawater)

    // Ensure SD card is working
    if (!SD.begin(SDCardCS)) {
        Serial.println("initialization failed!");
        return;
    }
    Serial.println("initialization done.");

    print_wakeup_reason();
    Serial.println("The time is " + Rtc.now().timestamp());

    Lidar();
    temperature();
    pressure();

    myFile = SD.open("/data.txt", "a");
    // if the file opened okay, write to it:
    if (myFile) {
        Serial.print("Writing to data.txt...");
        myFile.println("Time:" + Rtc.now().timestamp());
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
        Serial.println("error writing to data.txt");
    }

    // re-open the file for reading:
    myFile = SD.open("/data.txt");
    if (myFile) {
        Serial.println("data.txt:");

        // read from the file until there's nothing else in it:
        while (myFile.available()) {
            Serial.write(myFile.read());
        }
        // close the file:
        myFile.close();
    } else {
        // if the file didn't open, print an error:
        Serial.println("error opening data.txt");
    }

    digitalWrite(D12, LOW);
    digitalWrite(D10, LOW);
    digitalWrite(D11, LOW);
    digitalWrite(D6, LOW);

    esp_deep_sleep_start();
    Serial.println("This shouldnt print");
}

void loop() {
    // if (alarm_triggered) {
    //     Serial.println("Alarm triggered.\n");

    //     setAlarmInterval(1);
    //     // Set the RTC to the current time

    //     Rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // } else {
    //     Serial.println("Alarm not triggered. The time is " + Rtc.now().timestamp());

    //     delay(1000);
    // }
}
