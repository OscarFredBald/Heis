#pragma once
#include <Arduino.h>
#include "Config.h"

// "Queue" brukes her som planlegger som oppfyller REQ16.
// Den holder oversikt over hvilke etasjer som har aktive forespørsler
// (både fra heisens innside og fra hall-knappene utenfor).
// Det er altså heisens "oppgavekø".
class Queue {
public:
  Queue(); // Konstruktør – initialiserer køen (alle forespørsler settes til false)

  void queue_setup();   // Kalles i setup() (Elevator.cpp) for å initialisere datastrukturen (typisk nullstille arrays)

  // legg til forespørsler
  void add_inside(int floor);    // Når passasjeren trykker på knapp for etasje i heisen
  void add_hall_up(int floor);   // Når noen i gangen trykker på "opp"-knappen i gitt etasje
  void add_hall_down(int floor); // Når noen trykker på "ned"-knappen i gitt etasje

  // Hovedfunksjon
  // Kalles jevnlig av hovedprogrammet for å finne ut hvor heisen skal kjøre neste gang.
  // Den bestemmer neste target-etasje i henhold til krav REQ16 (SCAN-prinsippet).
  // Returnerer -1 hvis det ikke finnes noen forespørsel.
  int  queue(int current_floor, Dir &direction);

  // RYDDEFUNKSJON
  void clear_at(int floor); // Kalles når døren er helt åpen (REQ13) for å slette forespørsler i den etasjen.

   // STATUSFUNKSJONER
  bool any_above(int floor) const;  // Returnerer true hvis det finnes en forespørsel over gjeldende etasje
  bool any_below(int floor) const;  // Returnerer true hvis det finnes en forespørsel under gjeldende etasje
  bool any_requests() const;        // Returnerer true hvis det finnes *noen som helst* forespørsel

  int target_floor;  // Valgfritt felt som kan lagre målet (nyttig for debugging / visualisering)
 
private:  //HJELPEFUNKSJONER OG DATA
  
  // Konverterer etasje-nummer (MIN_FLOOR..MAX_FLOOR) til en gyldig indeks (0..N-1)
  // Eksempel: hvis MIN_FLOOR=1 og du spør om idx(3) -> returnerer 2
  int idx(int floor) const { return floor - MIN_FLOOR; } 

  // Arrays som representerer hvilke etasjer som har aktive forespørsler.
  // inside[f] = knapp trykket i heisen
  // hall_up[f] = noen vil opp fra denne etasjen
  // hall_down[f] = noen vil ned fra denne etasjen
  bool inside[MAX_FLOOR - MIN_FLOOR + 1];
  bool hall_up[MAX_FLOOR - MIN_FLOOR + 1];
  bool hall_down[MAX_FLOOR - MIN_FLOOR + 1];

  // Velger neste mål i nåværende retning, eller snur hvis ingen flere forespørsler
  // Implementerer SCAN-prinsippet: heisen fortsetter i samme retning så lenge mulig.
  int pick_in_direction(int current_floor, Dir dir) const;
};
