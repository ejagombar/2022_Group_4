#ifndef ERROR_H
#define ERROR_H

#include <Arduino.h>

typedef int8_t Error;

#define NO_ERROR 0
#define FATAL_ERROR -1
#define UNSTABLE_ERROR -2

#endif  // ERROR_H