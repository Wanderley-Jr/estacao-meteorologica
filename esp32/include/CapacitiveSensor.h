#ifndef CAPACITIVE_SENSOR_H
#define CAPACITIVE_SENSOR_H

#include <Arduino.h>

/* 
 * Common code for capacitive sensors (rain, soil humidity and photovoltaic)
 */
class CapacitiveSensor {
    private:
        const int pin;
    public:
        CapacitiveSensor(int pin);
        float read();
};

#endif