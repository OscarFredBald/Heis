#pragma once
#include <Arduino.h>

class Step_motor {
public:
 // enA/phA = Enable/Phase for coil A, enB/phB = Enable/Phase for coil B (magnetfelt (stepmotor))
  // Step_motor (); - Konstruktør som mottar hvilke pinner (på mikrokontrolleren) som brukes til å styre spoler A og B,
  // samt hvor mange steg (steps_per_action) motoren skal bevege seg per "handling" (f.eks. åpne/lukke dør)
  Step_motor(uint8_t enA, uint8_t phA, uint8_t enB, uint8_t phB, int steps_per_action);


  void step_motor_setup(); // Setter opp motorens pinner (konfigurerer pinMode til OUTPUT osv.)
  void opendoor();   // Kjører motoren slik at døren åpnes (f.eks. roterer motoren i én retning)
  void closedoor();  // Kjører motoren slik at døren lukkes (roterer motsatt vei)

  void stepCW(int s);   // Utfør 's' antall steg med rotasjon med klokka (CW = clockwise)
  void stepCCW(int s);  // Utfør 's' antall steg mot klokka (CCW = counter-clockwise)

  void invertA(bool inv) { _invA = inv; }  // Valgfritt: Lar deg invertere retningen på coil A (bytte ledninger programmatisk)
  void invertB(bool inv) { _invB = inv; }   // Valgfritt: Lar deg invertere retningen på coil B (bytte ledninger programmatisk)

private:
  // Setter logiske verdier på fasepinnene (phA og phB) basert på aktuell fase
  // Hver fase definerer hvilken kombinasjon av spoler som skal aktiveres.
  void applyPhase(int phase);      

  // Utfører 'n' steg i ønsket retning (cw = med klokka).
  // Husk å oppdatere hvilken fase som er aktiv mellom hvert kall.
  void doSteps(int n, bool cw);    


  // Konfigurasjonsvariabler
  uint8_t _enA, _phA, _enB, _phB;
  int     steps;

 // Tilstandsvariabler 
  int  _phase = 0;     // Hvilken fase motoren står i nå (0..3
  bool _invA  = true;  // Om coil A er invertert (true = bytt polaritet)
  bool _invB  = true;  // Om coil B er invertert (true = bytt polaritet)
};
