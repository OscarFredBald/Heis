#include "Encoder.h"

Encoder* Encoder::instance = nullptr;

Encoder::Encoder(uint8_t pinA, uint8_t pinB)
: encoder_position(0), _pinA(pinA), _pinB(pinB) {}

void Encoder::encoder_setup() {
  instance = this;
  pinMode(_pinA, INPUT_PULLUP);
  pinMode(_pinB, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(_pinA), Encoder::isrA, CHANGE);
  attachInterrupt(digitalPinToInterrupt(_pinB), Encoder::isrB, CHANGE);
}

void Encoder::isrA() { if (instance) instance->signalA(); }
void Encoder::isrB() { if (instance) instance->signalB(); }

void Encoder::signalA() {
  bool a = digitalRead(_pinA);
  bool b = digitalRead(_pinB);
  encoder_position += (a == b) ? +1 : -1;
}
void Encoder::signalB() {
  bool a = digitalRead(_pinA);
  bool b = digitalRead(_pinB);
  encoder_position += (a != b) ? +1 : -1;
}

long Encoder::get_position() const { return encoder_position; }
