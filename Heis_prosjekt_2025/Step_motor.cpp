#include "Step_motor.h"
#include <Arduino.h>
#include <SPI.h>

// (Valgfritt – fjern hvis du ikke bruker DAC et annet sted)
// Denne funksjonen sender et 12-bit signal (0–4095) til en DAC (MCP4922 eller lignende)
// via SPI. Den brukes hvis du vil generere analoge spenninger (f.eks. til motorstyring).
void dacWrite(uint16_t val) {           // val = 0..4095
  digitalWrite(SS, LOW);                //Aktiver SPI-enheten (chip select lav)

  // Første byte inneholder kontrollbiter og de øverste 4 bitene av 'val'.
  // 0x30 = 0011 0000 -> betyr: kanal A, buffered input, gain=1x, aktiv DAC
  // (val >> 8) & 0x0F tar de øverste 4 bitene av val.
  SPI.transfer(0x30 | ((val >> 8) & 0x0F)); 
  SPI.transfer(val & 0xFF);       // Send de nederste 8 bitene
  digitalWrite(SS, HIGH);         // Deaktiver SPI-enheten (chip select høy)
}

// Konstruktør – lagrer hvilke pinner som brukes for hver spole, og hvor mange steg motoren skal ta per “handling”.
Step_motor::Step_motor(uint8_t enA, uint8_t phA, uint8_t enB, uint8_t phB, int steps_per_action)
: _enA(enA), _phA(phA), _enB(enB), _phB(phB), steps(steps_per_action) {
  // Dersom motoren bare roterer i én retning, kan du invertere en av spolene.
  // Dette endrer polariteten elektronisk i stedet for å bytte ledninger fysisk.
  // _invB = true;  // (eller _invA = true)
}

// Initialiserer motorpinnene slik at de kan brukes som utganger.
void Step_motor::step_motor_setup() {
  pinMode(_enA, OUTPUT); pinMode(_phA, OUTPUT);
  pinMode(_enB, OUTPUT); pinMode(_phB, OUTPUT);

  // Slår på begge spoler (Enable HIGH) slik at motoren er klar til bruk.
  digitalWrite(_enA, HIGH);
  digitalWrite(_enB, HIGH);

 // Starter i fase 0 (definert startposisjon)
  _phase = 0;
  applyPhase(_phase); // Setter riktige signaler på spolepinnene
}

// Gi åpen/lukk hver sin retning
void Step_motor::opendoor()  { stepCW(200/2); } // Åpner døren ved å rotere motoren et gitt antall steg(100-halv rotasjon) med klokka (CW)
void Step_motor::closedoor() { stepCCW(200/2); } // // Lukker døren ved å rotere motoren samme antall steg mot klokka (CCW)

void Step_motor::stepCW(int s)  { doSteps(s, true);  } // Utfør s steg med klokka
void Step_motor::stepCCW(int s) { doSteps(s, false); } // Utfør s steg mot klokka

// Setter spolesignaler for gitt fase (0..3). Dette styrer hvilken coil som trekkes.
void Step_motor::applyPhase(int phase) {
 // Fase 0–3 angir hvilken kombinasjon av coil-polariteter som er aktiv.
  // Hver fase flytter motoren ett trinn frem eller tilbake.
  // Tabell:
  // 0: (phA = +, phB = -)
  // 1: (phA = -, phB = -)
  // 2: (phA = -, phB = +)
  // 3: (phA = +, phB = +)
  bool phA, phB;
  switch (phase & 3) {  // & 3 sørger for at fase alltid blir mellom 0 og 3
    case 0: phA = HIGH; phB = LOW;  break;
    case 1: phA = LOW;  phB = LOW;  break;
    case 2: phA = LOW;  phB = HIGH; break;
    default: // case 3
            phA = HIGH; phB = HIGH; break;
  }

  // Hvis vi har aktivert invertering for coil A eller B, snur vi polariteten her
  if (_invA) phA = !phA;
  if (_invB) phB = !phB;

 // Sender signalene ut til motorpinnene
  digitalWrite(_phA, phA);
  digitalWrite(_phB, phB);


   // Holder Enable-pinnene aktive (HIGH) slik at spoler er påslått.  
  digitalWrite(_enA, HIGH);
  digitalWrite(_enB, HIGH);
}


// Utfør 'n' steg i valgt retning (cw = true for med klokka)
// Hver iterasjon endrer fase og oppdaterer spolesignalene.
void Step_motor::doSteps(int n, bool cw) {
  const int dly = 4000; // forsinkelse mellom hvert steg i mikrosekunder (4000 µs = 4 ms)
                        // Lavere verdi = raskere rotasjon, men mindre moment
  for (int k = 0; k < n; ++k) { // Øker eller reduserer fasen for å bevege oss i riktig retning
    _phase += cw ? +1 : -1;
    applyPhase(_phase); // Setter utgangene for den nye fasen
    delayMicroseconds(dly); // Venter litt før neste steg
  }
}



