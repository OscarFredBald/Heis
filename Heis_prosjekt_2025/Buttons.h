#pragma once
#include <Arduino.h>
#include "Config.h"

// Hall buttons
class Hall_buttons {
public:
  void hall_buttons_setup();     // sett opp hall-knapper (pinMode, pullups) –
  int  hall_buttons();           // les knappetrykk; returner -1 hvis ingen, ellers etasje 1..4
};

// Inside (elevator) buttons
class Elevator_buttons {
public:
  explicit Elevator_buttons(const uint8_t pins[4]); // konstruktør: mottar 4 pin-numre for innvendige etasjeknapper
  void setup();                                     // kall i setup(): konfigurerer pinner som INPUT_PULLUP og nullstiller historikk
  int  read_pressed();                              // edge-detect: returnerer -1 hvis ingen ny trykk-edge, ellers etasje 1..4
private:
  uint8_t _pins[4];                                 // lagrer tilhørende pinnummer for knapp 1..4
  bool    _last[4];                                 // forrige stabile nivå for hver knapp (til edge-detect)
  unsigned long _last_ms[4];                        // siste tidsstempel for hver knapp (for debounce)
  static const unsigned long DEBOUNCE_MS = 25;      // debounce-vindu i millisekunder for innvendige knapper
};

// Emergency button (latched)
class Emergency_button {
public:
  explicit Emergency_button(uint8_t pin);           // konstruktør: lagrer hvilken pin nødknappen bruker
  void emergency_setup();                           // kall i setup(): konfigurer INPUT_PULLUP og initierer intern tilstand
  void emergency_update();                          // kalles jevnlig i loop(): toggler _active på FALLING edge (med debounce)
  bool is_active() const { return _active; }        // les latch status: true når nødmodus er aktiv
private:
  uint8_t _pin;                                     // pinnummer til nødknappen 
  bool    _active;                                  // latchet tilstand: blir værende true etter trykk til neste toggle
  bool    _last_level;                              // forrige stabile nivå lest fra pin (for å detektere FALLING edge (H->L))
  unsigned long _last_ms;                           // sist tid vi godtok en edge (for debounce)
  static const unsigned long DEBOUNCE_MS = 30;      // debounce-vindu for nødknappen i millisekunder
};

class Buttons {
public:
  // Konstruktør: tar inn pinner for innvendige knapper (4 stk) og emergency-pin
  explicit Buttons(const uint8_t pins_inside[4], uint8_t emergency_pin)
  : elev(pins_inside),           // initierer innvendige knapper med pin-array
    emergency(emergency_pin) {}  // initierer emergency-knapp med pin

  Hall_buttons     hall;         // hall-knapper (kan være tastatur-basert implementasjon)
  Elevator_buttons elev;         // innvendige etasjeknapper (debounce + edge)
  Emergency_button emergency;    // nødknapp (latchet + debounce)
};

