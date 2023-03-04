#ifndef Saved_H
#define Saved_H

#include <Arduino.h>
#include <Vector>

class Saved {
   private:
    
      public:
    Saved(){};
    ~Saved(){};
    void AddDevice();
    uint8_t GetDeviceCount();
};

#endif Saved_H