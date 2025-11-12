#pragma once
#include <Arduino.h>
#include "Config.h"

class LEDs {
public:
 // Konstruktør: mottar en liste (array) med 4 pin-numre som brukes til LED-ene.
  // Disse LED-ene representerer typisk etasjene i heisen (f.eks. 4 etasjer).
  LEDs(const uint8_t pins[4]);

  void LED_setup();  // Kalles i setup() for å konfigurere LED-pinnene som utganger.
  void floor_indicator(int floor); // Slår på LED som tilsvarer aktuell etasje og slår av de andre.  f.eks. hvis floor = 2, så lyser kun LED for etasje 2.
  int  floor_number;  // Holder styr på hvilken etasje som sist ble vist.
private:
  uint8_t _pins[4]; // Lagrer pin-numrene til de 4 LED-ene som brukes til etasjeindikator.
};
