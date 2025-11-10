#include "Buttons.h"

// -------- Hall (fortsatt stub til du legger inn pinner) --------
void Hall_buttons::hall_buttons_setup() {}
int  Hall_buttons::hall_buttons() { return -1; }

// -------- Inside (elevator) --------
Elevator_buttons::Elevator_buttons(const uint8_t pins[4]) {
  for (int i=0;i<4;++i) { _pins[i]=pins[i]; _last[i]=HIGH; _last_ms[i]=0; }
}
void Elevator_buttons::setup() {
  for (int i=0;i<4;++i) pinMode(_pins[i], INPUT_PULLUP); // active-LOW
}
int Elevator_buttons::read_pressed() {
  unsigned long now = millis();
  for (int f=1; f<=4; ++f) {
    int i = f-1;
    bool lvl = digitalRead(_pins[i]);      // LOW = pressed
    if (now - _last_ms[i] >= DEBOUNCE_MS) {
      // FALLING edge: HIGH -> LOW
      if (_last[i] == LOW && lvl == HIGH) { _last[i] = lvl; _last_ms[i] = now; return f; }
      _last[i] = lvl;
      _last_ms[i] = now;
    }
  }
  return -1;
}

// -------- Emergency (robust debounce + edge) --------
Emergency_button::Emergency_button(uint8_t pin)
: _pin(pin), _active(false), _last_level(HIGH), _last_ms(0) {}

void Emergency_button::emergency_setup() {
  pinMode(_pin, INPUT_PULLUP);
  _last_level = digitalRead(_pin);
  _last_ms = millis();
}

void Emergency_button::emergency_update() {
  unsigned long now = millis();
  if (now - _last_ms < DEBOUNCE_MS) return; // lås i debounce-vindu

  bool level = digitalRead(_pin);
  if (_last_level == HIGH && level == LOW) {
    _active = !_active;                     // toggle på FALLING edge
  }
  _last_level = level;
  _last_ms = now;
}
