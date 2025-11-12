#pragma once
#include <Arduino.h>
#include <LiquidCrystal.h>

class LCD {
public:
  LCD(uint8_t rs, uint8_t e, uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7);


  // SETUP 
  // Kalles i Arduino sin setup() for å initialisere LCD-displayet:
  //  - starter kommunikasjon
  //  - definerer hvor mange kolonner og rader displayet har (f.eks. 16x2)
  void LCD_setup();

  // VIS ETASJE 
  // Viser gjeldende etasje på LCD-skjermen, f.eks. "Floor: 3"
  void show_floor(int f);

   // VIS MELDING
  // Skriver en tekststreng på LCD-displayet, f.eks. "Door Opening..."
  void show_msg(const char* txt);
private:
  // brukes til å kommunisere med selve LCD-skjermen.
  LiquidCrystal _lcd;
};
