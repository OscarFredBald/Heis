#include <Arduino.h>
#include "Overweight.h"

Overweight::Overweight(uint8_t analog_pin, int threshold)
: _pin(analog_pin), _thresh(threshold), potensiometer_value(0) {}

void Overweight::overweight_setup() {
  pinMode(_pin, INPUT);
}

bool Overweight::overweight() {
  potensiometer_value = analogRead(_pin);
  return (potensiometer_value > _thresh);
}
