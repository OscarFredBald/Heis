#include "LCD.h"                        

// Konstruktør: tar imot pin-numre for RS, E og D4–D7 og sender dem videre til LCD
LCD::LCD(uint8_t rs, uint8_t e, uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7)
: _lcd(rs, e, d4, d5, d6, d7) {}        // bruker initializer-list til å opprette det interne _lcd-objektet

// Kalles i setup() for å starte LCD-skjermen
void LCD::LCD_setup() {
  _lcd.begin(16, 2);                    // initialiser LCD-modulen (16 kolonner, 2 rader)
  _lcd.clear();                         // tøm skjermen
  _lcd.print("Heis klar");              // skriv velkomstmelding på linje 1
}

// Viser nåværende etasje (f)
void LCD::show_floor(int f) {
  _lcd.setCursor(0, 0);                 // flytt markøren til øverste linje, første kolonne
  _lcd.print("Etasje:      ");          // skriv teksten "Etasje:" og fyll ut litt mellomrom for å slette gamle tall
  _lcd.setCursor(8, 0);                 // flytt markøren dit tallet skal stå
  _lcd.print(f);                        // skriv etasjenummeret
}

// Viser en melding på linje 2 (status eller info)
void LCD::show_msg(const char* txt) {
  _lcd.display();
  _lcd.noBlink();
  _lcd.noCursor();
  
  
  _lcd.setCursor(0, 1);                 // flytt til andre linje
  _lcd.print("                ");       // skriv 16 mellomrom for å slette tidligere tekst
  _lcd.setCursor(0, 1);                 // flytt tilbake til starten av linjen
  _lcd.print(txt);                      // skriv ny melding
}
