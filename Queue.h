#pragma once
#include <Arduino.h>
#include "Config.h"

// "Queue" i ditt diagram brukes her som planlegger som oppfyller REQ16.
// Vi holder tre sett med forespørsler for MIN_FLOOR..MAX_FLOOR:
//  - inside[f], hall_up[f], hall_down[f]
class Queue {
public:
  Queue();

  void queue_setup();

  // legg til forespørsler
  void add_inside(int floor);
  void add_hall_up(int floor);
  void add_hall_down(int floor);

  // kalles jevnlig for å bestemme target i henhold til REQ16
  // returnerer -1 hvis ingen forespørsel
  int  queue(int current_floor, Dir &direction);

  // Sletter forespørsler på en etasje (REQ13) når dør er helt åpen
  void clear_at(int floor);

  // Hjelp / status
  bool any_above(int floor) const;
  bool any_below(int floor) const;
  bool any_requests() const;

  int target_floor; // valgfri "state"

private:
  // indeks fra MIN_FLOOR..MAX_FLOOR -> 0..(N-1)
  int idx(int floor) const { return floor - MIN_FLOOR; }
  bool inside[MAX_FLOOR - MIN_FLOOR + 1];
  bool hall_up[MAX_FLOOR - MIN_FLOOR + 1];
  bool hall_down[MAX_FLOOR - MIN_FLOOR + 1];

  // velg neste i nåværende/andre retning (SCAN)
  int pick_in_direction(int current_floor, Dir dir) const;
};
