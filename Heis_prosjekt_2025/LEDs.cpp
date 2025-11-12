#include "LEDs.h"



//  KONSTRUKTØR 
// Konstruktøren mottar et array med 4 pin-numre (én pin per LED).
// floor_number settes til MIN_FLOOR slik at heisen starter "på bunn" (etasje 1)
LEDs::LEDs(const uint8_t pins[4]) : floor_number(MIN_FLOOR) {  // Kopierer pin-numrene fra parameter-arrayet 'pins' til det interne feltet _pins
  for (int i=0;i<4;++i) // Kopierer pin-numrene fra parameter-arrayet 'pins' til det interne feltet _pins
  _pins[i]=pins[i];  // Lagrer hver pin i objektets egen _pins-array
}

// SETUP
// Kalles i Arduino sin setup() for å klargjøre LED-pinnene.
void LEDs::LED_setup() { // Løkke som går gjennom alle fire LED-pinnene
  for (int i=0;i<4;++i) { 
    pinMode(_pins[i], OUTPUT);  // Setter hver pin som utgang
    digitalWrite(_pins[i], LOW); } // Slår av alle LED-ene ved oppstart
}

// ETASJE-INDIKATOR
// Viser hvilken etasje heisen er i ved å slå på riktig LED og slå av alle andre.
void LEDs::floor_indicator(int floor) { // Sikkerhetssjekk: gjør ingenting hvis etasjen er utenfor gyldig område
  if (floor<MIN_FLOOR || floor>MAX_FLOOR) return;

  for (int i=0;i<4;++i) // Slå av alle LED-er først
  digitalWrite(_pins[i], LOW);

  // Slå på LED-en som tilsvarer den aktuelle etasjen.
  // (floor - MIN_FLOOR) brukes for å finne riktig indeks i arrayet.
  digitalWrite(_pins[floor-MIN_FLOOR], HIGH);
  
  // Lagre hvilken etasje som sist ble vist (nyttig for debugging/visning)
  floor_number = floor;
  
}
