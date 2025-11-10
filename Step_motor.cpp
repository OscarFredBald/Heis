#include "Step_motor.h"
#include <Arduino.h>
#include <SPI.h>

// (Valgfritt – fjern hvis du ikke bruker DAC et annet sted)
// Hvis du faktisk bruker dette, husk SPI.begin() og pinMode(SS, OUTPUT) i setup.
void dacWrite(uint16_t val) {           // 0..4095
  digitalWrite(SS, LOW);
  SPI.transfer(0x30 | ((val >> 8) & 0x0F)); // MCP4922: ch A, buf, gain=1, active
  SPI.transfer(val & 0xFF);
  digitalWrite(SS, HIGH);
}

Step_motor::Step_motor(uint8_t enA, uint8_t phA, uint8_t enB, uint8_t phB, int steps_per_action)
: _enA(enA), _phA(phA), _enB(enB), _phB(phB), steps(steps_per_action) {
  // Hvis motoren kun funket i én retning hos dere, kan du prøve å invertére én coil:
  // _invB = true; // eller _invA = true;
}

void Step_motor::step_motor_setup() {
  pinMode(_enA, OUTPUT); pinMode(_phA, OUTPUT);
  pinMode(_enB, OUTPUT); pinMode(_phB, OUTPUT);

  // Full "på". Vil du styre moment: bruk analogWrite(_enX, duty) på PWM-pinner.
  digitalWrite(_enA, HIGH);
  digitalWrite(_enB, HIGH);

  // Definert startfase
  _phase = 0;
  applyPhase(_phase);
}

// Gi åpen/lukk hver sin retning
void Step_motor::opendoor()  { stepCW(200/2); } // Bestemmer hvor mange steps motoren roterer
void Step_motor::closedoor() { stepCCW(200/2); } // Bestemmer hvor mange steps motoren roterer. Det sto stepCCW(steps)

void Step_motor::stepCW(int s)  { doSteps(s, true);  }
void Step_motor::stepCCW(int s) { doSteps(s, false); }

// -------------------- ROBUST FASE-KONTROLL --------------------
// Fullstep-tabel: 4 delsteg. Vi husker _phase og går alltid til nabo-fase.
// Det hindrer “ping-pong” ved retningsbytte.
void Step_motor::applyPhase(int phase) {
  // Fase 0..3: (phA, phB)
  // 0: (+, -)
  // 1: (-, -)
  // 2: (-, +)
  // 3: (+, +)
  bool phA, phB;
  switch (phase & 3) {
    case 0: phA = HIGH; phB = LOW;  break;
    case 1: phA = LOW;  phB = LOW;  break;
    case 2: phA = LOW;  phB = HIGH; break;
    default: // 3
            phA = HIGH; phB = HIGH; break;
  }

  // Eventuell programmatisk polaritetsinversjon
  if (_invA) phA = !phA;
  if (_invB) phB = !phB;

  digitalWrite(_phA, phA);
  digitalWrite(_phB, phB);
  // EN holdes HIGH (evt. analogWrite for PWM)
  digitalWrite(_enA, HIGH);
  digitalWrite(_enB, HIGH);
}

void Step_motor::doSteps(int n, bool cw) {
  const int dly = 4000; // µs – øk for mer moment/lavere hastighet
  for (int k = 0; k < n; ++k) {
    _phase += cw ? +1 : -1;
    applyPhase(_phase);
    delayMicroseconds(dly);
  }
}



