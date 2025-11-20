#pragma once
#include <Arduino.h>


// Klassen "Encoder" brukes for å lese signaler fra en rotasjonsenkoder.
// En enkoder gir to pulssignaler (A og B) som forskyves i fase for å avgjøre retning.
// Hver endring i signalene betyr at motoren/akslingen har beveget seg et lite steg.
class Encoder {
public:
  // KONSTRUKTØR
  // Tar inn to digitale pin-numre: A og B, som kobles til enkoderens utganger.
  Encoder(uint8_t pinA, uint8_t pinB);

  // SETUP
  // Kalles i setup()-delen av programmet.
  // Setter opp pinMode() for enkoder-pinnene og aktiverer eventuelle interrupts (ISR-er)
  void  encoder_setup();

  // POSISJON 
  // Returnerer den gjeldende posisjonen til enkoderen.
  // Typisk øker denne når akslingen roterer en vei, og minker i motsatt.
  long  get_position() const;

  // Signaler
  // Disse to funksjonene kalles fra interrupt-rutinene (ISR-er)
  // når signal A eller signal B endrer tilstand (HIGH/LOW).
  // De oppdaterer intern posisjon basert på fasen mellom A og B.
  void  signalA();  // ISR
  void  signalB();  // ISR

  // POSISJONSVERDI
  // Denne variabelen lagrer hvor mange "ticks" (pulser) enkoderen har registrert.
  // Den kan være positiv eller negativ, avhengig av rotasjonsretning.
  volatile long encoder_position; //forteller kompilatoren at verdien kan endres “utenfor synsvidde (volatile)


  // STATISKE ISR-PEKERE
  // Arduino krever at ISR-funksjoner er "static" (de kan ikke tilhøre et bestemt objekt).
  // Derfor lages statiske "bro-funksjoner" som peker til riktig Encoder-instans.
  // instance peker til det ene Encoder-objektet som brukes.
  static void isrA();       // Kalles automatisk når pin A endrer tilstand
  static void isrB();       // Kalles automatisk når pin B endrer tilstand
  static Encoder* instance; // Static peker slik at ISR-funksjonene vet hvilken encoder de tilhører
 
private:
  // PINNER
  // Lagrer hvilke pinner som brukes til A og B-kanalene på enkoderen.
  // Disse er brukt både i setup() og i ISR-funksjonene for å lese signalstatus.
  uint8_t _pinA,_pinB;
};
