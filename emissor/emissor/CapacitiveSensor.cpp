#include <Arduino.h>

/* 
 * Common code for capacitive sensors (rain, soil humidity and photovoltaic)
 */
class CapacitiveSensor {
    int pin;

    public:
        CapacitiveSensor(int pin) {
            this->pin = pin;
        }

        float read() {
            return map(analogRead(pin), 0, 4095, 100, 0);
        }
};