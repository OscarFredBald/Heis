#pragma once
#include <Arduino.h>

class Overweight { // Klassen "Overweight" brukes for å måle om heisen er overbelastet. ved å lese fra en analog sensor potensiometer
public:
// Konstruktør:
  //  - analog_pin: pinnen på Arduinoen der sensoren er koblet til (A0, A1, osv.)
  //  - threshold: grenseverdi (0–1023) som definerer når det regnes som overvekt.
  Overweight(uint8_t analog_pin, int threshold = 700);
  void overweight_setup();      // Kalles i setup() for å konfigurere pinnen (analog inngang)
  bool overweight();            // Måler sensorverdien og returnerer true hvis verdien > terskelverdi (overvekt)
  int  potensiometer_value;     // Lagrer siste målte analogverdi (0–1023 fra Arduino ADC)

private:
  uint8_t _pin; // Hvilken analog pin som brukes til målingen
  int     _thresh; // Terskelverdi for når overvekt skal registreres
}; 
