#include <Arduino.h>
#include "Overweight.h"

// Konstruktør
// Denne funksjonen kjøres automatisk når det opprettes et nytt Overweight-objekt.
// Den lagrer hvilken analog pin som brukes, hvilken terskelverdi (threshold) som gjelder,
// og setter startverdien til potensiometer_value = 0.
Overweight::Overweight(uint8_t analog_pin, int threshold)
: _pin(analog_pin),       // _pin lagrer hvilken analog inngang som brukes (f.eks. A0)
_thresh(threshold),       // _thresh lagrer grenseverdien for hva som regnes som "overvekt"
potensiometer_value(0) {} // starter med verdi 0 før noen måling er gjort

// SETUP
// Kalles i Arduinoens setup()-funksjon.
// Setter den valgte pinnen (_pin) som en inngang slik at vi kan lese analoge verdier.
void Overweight::overweight_setup() {
  pinMode(_pin, INPUT); // Konfigurerer pinnen som analog inngang
}

// MÅLING 
// Denne funksjonen leser verdien fra sensoren og sjekker om den overstiger terskelen.
// Returnerer:
// True  = heisen er for tung / sensoren viser over terskelverdi
// False = alt er innenfor normal vekt
bool Overweight::overweight() {
  potensiometer_value = analogRead(_pin); // Les analogt signal fra sensoren (verdi mellom 0 og 1023)
  // Sammenlign med terskelen (_thresh)
  // Hvis verdien er større enn terskelen -> "overweight" = true
  return (potensiometer_value > _thresh);
  
}
