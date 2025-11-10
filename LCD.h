#pragma once
#include <Arduino.h>
#include <LiquidCrystal.h>

class LCD {
public:
  LCD(uint8_t rs, uint8_t e, uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7);
  void LCD_setup();
  void show_floor(int f);
  void show_msg(const char* txt);
private:
  LiquidCrystal _lcd;
};
