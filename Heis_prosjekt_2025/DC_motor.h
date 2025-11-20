#pragma once
#include <Arduino.h>

class DC_motor {
public:
  // Konstruktør: tar inn pin-nummer for driverens ENABLE (PWM), PHASE (retning) og DECAY (bremse-/decay-modus)
  DC_motor(uint8_t pin_enable, uint8_t pin_phase, uint8_t pin_decay); 

  // Kalles i setup(): konfigurerer pinnene (OUTPUT), setter starttilstand (motor av, nuller PID-variabler)
  void dc_motor_setup();  

  // En PID-oppdatering per tidssteg: beregner reguleringssignal basert på posisjonsfeil og kaller motor_control()
  // current_position = målt posisjon (fra encoder), dt_s = tidssteg i sekunder'
  void update_pid(long current_position, float dt_s);

  // Sender reguleringssignal til motoren: retning via PHASE, hastighet via PWM på ENABLE, ev. DECAY-modus
  // control = ønsket “u” (kan være positiv/negativ), skaleres/kappes til 0..motor_speed_max
  void motor_control(float control);

  // Slår av motoren umiddelbart (ENABLE=0), beholder gjerne retning uendret
  void stop();

  long  desired_position;    // Ønsket posisjon motoren skal regulere mot (settpunkt)
  float control_signal;      // Valgfri lagring av siste utregnede kontrollsignal (for logging/diagnostikk)
  int   motor_speed_max;     // Øvre grense for PWM-hastighet (0..255 for analogWrite på Arduino)
  float pid_signal;          // PID-utgang “u” (samme som control_signal, men beholdt for tydelighet/feilsøking)
  float Kp, Ki, Kd;          // PID-parametere: proporsjonal (Kp), integrerende (Ki), deriverende (Kd)

private:
  uint8_t _pin_enable, _pin_phase, _pin_decay;  // Lagrer tilkoblede pinner: ENABLE (PWM), PHASE (retning), DECAY (bremse-/freewheel-modus på driver)
  long  _e_prev; // Forrige feil e[k-1] (brukes i D-ledd og for differanseberegning)
  float _e_int;  // Integrert feil (sum e*dt) for I-leddet; anti-windup bør håndteres i update_pid()
};
