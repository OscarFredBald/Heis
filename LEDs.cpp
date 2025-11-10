#include "LEDs.h"

LEDs::LEDs(const uint8_t pins[4]) : floor_number(MIN_FLOOR) {
  for (int i=0;i<4;++i) _pins[i]=pins[i];
}
void LEDs::LED_setup() {
  for (int i=0;i<4;++i) { pinMode(_pins[i], OUTPUT); digitalWrite(_pins[i], LOW); }
}
void LEDs::floor_indicator(int floor) {
  if (floor<MIN_FLOOR || floor>MAX_FLOOR) return;
  for (int i=0;i<4;++i) digitalWrite(_pins[i], LOW);
  digitalWrite(_pins[floor-MIN_FLOOR], HIGH);
  floor_number = floor;
}
