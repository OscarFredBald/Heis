#pragma once
#include <Arduino.h>
#include "Config.h"
#include "LCD.h"
#include "Overweight.h"
#include "Queue.h"
#include "Buttons.h"
#include "Step_motor.h"
#include "Encoder.h"
#include "DC_motor.h"
#include "LEDs.h"

class Elevator {
public:
  Elevator();
  void setup();  // init av alle delsystemer
  void loop();   // kjør kravlogikk (REQ1..16 med vekt på REQ16)
private:
  // "global Variables" i diagrammet tolkes som systemtilstand
  int  _current_floor;
  Dir  _direction;
  bool _door_closed;
  bool _servicing;  // true når vi er på vei mot en bestilt etasje


  // Komponenter
  LCD            _lcd;
  Overweight     _over;
  Queue          _queue;
  Buttons        _buttons;
  Emergency_button _emergency;
  Step_motor     _door;
  Encoder        _enc;
  DC_motor       _dc;
  LEDs           _leds;

  // Hjelp
  unsigned long  _prev_ms;

  // interne hjelpere
  void arrive_at_floor(int f);
  long floor_to_ticks(int f) const { return (long)( (f - MIN_FLOOR) * TICKS_PER_FLOOR ); }
  int  ticks_to_floor(long ticks) const; // enkel nearest
};



