#include <Arduino.h>

class Button {
   private:
    const uint8_t pin;
    bool ready = false;
    uint32_t timeOfPress = 0;
    void (*f)();

   public:
    static const uint8_t debounceTime = 200;

    Button(uint8_t pinIn, void (*func)()) : pin(pinIn), f(func) {}

    const uint8_t getPin() { return pin; }

    bool getReady() { return ready; }

    void pressed() {
        ready = false;
        timeOfPress = millis();
    }

    void setReady(bool ready) { this->ready = ready; }

    uint32_t getTimeOfPress() { return timeOfPress; }

    void callFunction() { f(); }
};