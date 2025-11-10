#include "LCD.h"

LCD::LCD(uint8_t rs, uint8_t e, uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7)
: _lcd(rs, e, d4, d5, d6, d7) {}

void LCD::LCD_setup() {
  _lcd.begin(16, 2);
  _lcd.clear();
  _lcd.print("Heis klar");
}

void LCD::show_floor(int f) {
  _lcd.setCursor(0, 0);
  _lcd.print("Etasje:      ");
  _lcd.setCursor(8, 0);
  _lcd.print(f);
}

void LCD::show_msg(const char* txt) {
  _lcd.setCursor(0, 1);
  _lcd.print("                ");
  _lcd.setCursor(0, 1);
  _lcd.print(txt);
}
