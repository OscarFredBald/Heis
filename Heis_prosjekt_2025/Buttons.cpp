#include "Buttons.h"                         

// === Hall_buttons.cpp ===
#include "Buttons.h"
#include <Arduino.h>

void Hall_buttons::hall_buttons_setup() {
}

// hall_buttons() skal lese tastaturet (f.eks. "2u" eller "3d")
// Returner -1 hvis ingen ny gyldig input
int Hall_buttons::hall_buttons() {
  static int pendingFloor = -1;  // husker tall før u/d
  while (Serial.available() > 0) {
    char c = tolower(Serial.read());           // les neste tegn og gjør små bokstaver
    int f = (c >= '0' && c <= '9') ? c - '0' : -1;  // sjekk om tallet er 0–9

    if (f != -1) {
      pendingFloor = f;                        // lagre valgt etasje
    } else if ((c == 'u' || c == 'd') && pendingFloor != -1) {
      int floor = pendingFloor;                // ferdig kombinasjon funnet
      pendingFloor = -1;                       // nullstill

      // valider etasjeområde
      if (floor < MIN_FLOOR || floor > MAX_FLOOR) return -1;

      // opprett forespørsel
      if (c == 'u' && floor < MAX_FLOOR) {
        // opp-forespørsel
        Serial.print("Hall Up "); Serial.println(floor);
        return floor;                           // returner etasjenummer
      } else if (c == 'd' && floor > MIN_FLOOR) {
        // ned-forespørsel
        Serial.print("Hall Down "); Serial.println(floor);
        return -floor;
      }
    }
  }
  return -1;  
}

// Inside (elevator)
Elevator_buttons::Elevator_buttons(const uint8_t pins[4]) {
  for (int i = 0; i < 4; ++i) {              // Gå gjennom alle fire knappene
    _pins[i]    = pins[i];                   // Lagre pinnummer for knapp i
    _last[i]    = HIGH;                      // Forrige nivå antas HIGH (INPUT_PULLUP → ikke trykket)
    _last_ms[i] = 0;                         // Ingen tidsstempel registrert ennå
  }
}
void Elevator_buttons::setup() {
  for (int i = 0; i < 4; ++i)                // For hver knapp
    pinMode(_pins[i], INPUT_PULLUP);         // Sett som INPUT_PULLUP (active-LOW: trykk = LOW)
}
int Elevator_buttons::read_pressed() {
  unsigned long now = millis();              // Hent “nå”-tid for debounce
  for (int f = 1; f <= 4; ++f) {             // Sjekk etasje-knapp 1..4
    int i = f - 1;                           // Array-indeks 0..3
    bool lvl = digitalRead(_pins[i]);        // Les nivå (LOW = trykket, HIGH = sluppet)
    if (now - _last_ms[i] >= DEBOUNCE_MS) {  // Debounce: kun vurder hvis det er gått nok tid
      if (_last[i] == LOW && lvl == HIGH) {  // Edge-deteksjon: forrige LOW og nå HIGH (slipp-øyeblikket)
        _last[i] = lvl;                      // Oppdater forrige nivå til nåværende
        _last_ms[i] = now;                   // Oppdater debounce-tidsstempel
        return f;                            // Rapporter knapp f (her på slipp-edge)
      }
      _last[i] = lvl;                        // Ingen edge → bare lagre nivå
      _last_ms[i] = now;                     // Oppdater debounce-tid
    }
  }
  return -1;                                 // Ingen ny edge registrert
}

// Emergency (robust debounce + edge)
Emergency_button::Emergency_button(uint8_t pin)
: _pin(pin),                                 // Lagre pinnummeret til nødknappen
  _active(false),                            // Start med inaktiv latchet tilstand
  _last_level(HIGH),                         // INPUT_PULLUP antas HIGH når ikke trykket
  _last_ms(0) {}                             // Ingen debounce-tidsstempel ved start

void Emergency_button::emergency_setup() {
  pinMode(_pin, INPUT_PULLUP);               // Sett knappen som INPUT_PULLUP (trykk → LOW)
  _last_level = digitalRead(_pin);           // Les og lagre første nivå som referanse
  _last_ms = millis();                       // Start debounce-tidsstempel
}

void Emergency_button::emergency_update() {
  unsigned long now = millis();              // Nåværende tid (ms)
  if (now - _last_ms < DEBOUNCE_MS) return;  // Debounce-lås: ignorer hvis for kort tid siden sist

  bool level = digitalRead(_pin);            // Les nåværende nivå (LOW når trykket)
  if (_last_level == HIGH && level == LOW) { // FALLING edge: HIGH→LOW (nytt trykk registrert)
    _active = !_active;                      // Toggle latchet status ved hvert trykk
  }
  _last_level = level;                       // Oppdater forrige nivå
  _last_ms = now;                            // Oppdater debounce-tidsstempel
}
