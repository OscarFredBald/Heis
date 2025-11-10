#pragma once
#include <Arduino.h>
#include "Config.h"

// ===== OPTIONAL: Hall (stub beholdes, men uten død state) =====
class Hall_buttons {
public:
  void hall_buttons_setup();     // implementer ved behov
  int  hall_buttons();           // -1 = ingen; ellers 1..4
};

// ===== Inside (elevator) buttons =====
class Elevator_buttons {
public:
  explicit Elevator_buttons(const uint8_t pins[4]);
  void setup();
  int  read_pressed();           // -1 = ingen; ellers 1..4 (edge-detect)
private:
  uint8_t _pins[4];
  bool    _last[4];
  unsigned long _last_ms[4];
  static const unsigned long DEBOUNCE_MS = 25;
};

// ===== Aggregator =====
class Buttons {
public:
  explicit Buttons(const uint8_t pins_inside[4]) : elev(pins_inside) {}
  Hall_buttons     hall;
 Elevator_buttons elev;
};

// ===== Emergency button (latched) =====
class Emergency_button {
public:
  explicit Emergency_button(uint8_t pin);
  void emergency_setup();
  void emergency_update();   // toggler på FALLING edge
  bool is_active() const { return _active; }
private:
  uint8_t _pin;
  bool    _active;
  bool    _last_level;
  unsigned long _last_ms;
  static const unsigned long DEBOUNCE_MS = 30;
};
