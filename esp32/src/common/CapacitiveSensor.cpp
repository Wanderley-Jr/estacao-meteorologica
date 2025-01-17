#include <CapacitiveSensor.h>

CapacitiveSensor::CapacitiveSensor(const int pin_): pin(pin_) {};

float CapacitiveSensor::read() {
	return map(analogRead(pin), 0, 4095, 100, 0);
}
