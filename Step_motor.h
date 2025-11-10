#pragma once
#include <Arduino.h>

class Step_motor {
public:
  // enA/phA = Enable/Phase for coil A, enB/phB = Enable/Phase for coil B
  Step_motor(uint8_t enA, uint8_t phA, uint8_t enB, uint8_t phB, int steps_per_action);

  void step_motor_setup();
  void opendoor();
  void closedoor();

  void stepCW(int s);
  void stepCCW(int s);

  // Valgfritt: programmatisk “bytt ledninger” ved å invertere polaritet for en coil
  void invertA(bool inv) { _invA = inv; }
  void invertB(bool inv) { _invB = inv; }

private:
  // Robust fase-styring (deklarasjoner som manglet)
  void applyPhase(int phase);      // Setter (phA, phB) basert på fase 0..3
  void doSteps(int n, bool cw);    // Trinn n delsteg, husk fase mellom kall

  // Konfig
  uint8_t _enA, _phA, _enB, _phB;
  int     steps;

  // Tilstand
  int  _phase = 0;      // gjeldende fase 0..3
  bool _invA  = true;  // inverter coil A-polaritet
  bool _invB  = true;  // inverter coil B-polaritet
};
