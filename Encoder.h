#pragma once
#include <Arduino.h>

class Encoder {
public:
  Encoder(uint8_t pinA, uint8_t pinB);
  void  encoder_setup();
  long  get_position() const;
  void  signalA();  // ISR
  void  signalB();  // ISR
  long encoder_position;

  static void isrA();
  static void isrB();
  static Encoder* instance;

private:
  uint8_t _pinA,_pinB;
};
