#pragma once
#include <Arduino.h>
#include "Config.h"

class LEDs {
public:
  LEDs(const uint8_t pins[4]);
  void LED_setup();
  void floor_indicator(int floor);
  int  floor_number;
private:
  uint8_t _pins[4];
};
