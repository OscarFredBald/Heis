#pragma once
#include <Arduino.h>

class Overweight {
public:
  Overweight(uint8_t analog_pin, int threshold = 700);
  void overweight_setup();
  bool overweight();            // true = overvekt
  int  potensiometer_value;     // sist leste (0..1023)

private:
  uint8_t _pin;
  int     _thresh;
};
