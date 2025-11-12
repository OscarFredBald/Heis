#include "Encoder.h"

// Oppretter og initialiserer den statiske peker-variabelen "instance" til null.
// Denne brukes slik at ISR-funksjonene vet hvilket Encoder-objekt de skal jobbe på.
Encoder* Encoder::instance = nullptr;

// Konstruktøren lagrer hvilke pinner som brukes for signal A og B,
// og setter startverdien for encoder_position = 0 (startposisjon).
Encoder::Encoder(uint8_t pinA, uint8_t pinB)
: encoder_position(0), _pinA(pinA), _pinB(pinB) {}


void Encoder::encoder_setup() {
  instance = this; // Gjør at de statiske ISR-funksjonene kan peke på dette objektet

  pinMode(_pinA, INPUT_PULLUP); // Sett A-kanal som inngang med intern pull-up motstand
  pinMode(_pinB, INPUT_PULLUP); // Sett B-kanal som inngang med intern pull-up motstand

  // Koble maskinvareavbrudd til ISR-funksjonene
  // Disse trigges hver gang signalet endrer tilstand (HIGH <->LOW)
  attachInterrupt(digitalPinToInterrupt(_pinA), Encoder::isrA, CHANGE);
  attachInterrupt(digitalPinToInterrupt(_pinB), Encoder::isrB, CHANGE);
}
// Statiske ISR-funksjoner som videresender signalet til riktig Encoder-instans
void Encoder::isrA() { if (instance) instance->signalA(); } // Kall signalA() på objektet hvis det finnes
void Encoder::isrB() { if (instance) instance->signalB(); } // Kall signalB() på objektet hvis det finnes
 
void Encoder::signalA() {
  bool a = digitalRead(_pinA); // Les nåværende tilstand på kanal A
  bool b = digitalRead(_pinB); // Les nåværende tilstand på kanal B

  // Hvis A og B har lik logisk verdi, øk posisjonen (rotasjon én vei)
  // Hvis de er forskjellige, minke posisjonen (rotasjon motsatt vei)
  encoder_position += (a == b) ? +1 : -1;
}

void Encoder::signalB() {
  bool a = digitalRead(_pinA); // Les nåværende tilstand på kanal A
  bool b = digitalRead(_pinB); // Les nåværende tilstand på kanal B

  // Her er logikken speilvendt sammenlignet med signalA().
  // Hvis A og B er ulike, øk posisjon; ellers minke.
  encoder_position += (a != b) ? +1 : -1;
}
// Returnerer den nåværende posisjonen til enkoderen (antall pulser)
long Encoder::get_position() const {
  noInterrupts();          // steng av avbrudd et kort øyeblikk
  long v = encoder_position;
  interrupts();            // på igjen
  return v;
}
//Dette er fikset nå î
 //Problem: ISR-ene (isrA/isrB) oppdaterer encoder_position mens loop() (hovedtråden) leser den. På 8-bit AVR (f.eks. Arduino Mega) er long 32-bit, 
 //og lesing kan skje i to CPU-operasjoner. Hvis en ISR avbryter midt i lesingen, kan du få en “revet” (torn) verdi.
