#include "DC_motor.h"
#include <Arduino.h>

// Konstruktør: initialiserer pinner, standardverdier og PID-tilstander
DC_motor::DC_motor(uint8_t pin_enable, uint8_t pin_phase, uint8_t pin_decay)
: _pin_enable(pin_enable),              // lagre PWM-/ENABLE-pin for hastighetsstyring
  _pin_phase(pin_phase),                // lagre PHASE-pin for retningsstyring
  _pin_decay(pin_decay),                // lagre DECAY-pin (bremse-/freewheel-modus på driver)
  desired_position(0),                  // start-settpunkt (ønsket posisjon) = 0
  control_signal(0.0f),                 // sist beregnede kontrollsignal (for logging/diagnose)
  motor_speed_max(255),                 // maks PWM
  pid_signal(0.0f),                     // PID-utgang u = 0 ved start
  Kp(0.01f), Ki(0.0005f), Kd(0.00f),    // PID-parametere
  _e_prev(0),                           // forrige feil e[k-1] = 0
  _e_int(0.0f)                          // integrert feil (sum e*dt) = 0
{}

// Oppsett av I/O-pinner og sikker starttilstand
void DC_motor::dc_motor_setup() {
  pinMode(_pin_enable, OUTPUT);         // ENABLE som utgang (PWM)
  pinMode(_pin_phase,  OUTPUT);         // PHASE som utgang (retning)
  pinMode(_pin_decay,  OUTPUT);         // DECAY som utgang (bremse-/freewheel-modus)

  digitalWrite(_pin_decay, LOW);        // typisk "fast decay"/freewheel
  analogWrite(_pin_enable, 0);          // motor av ved oppstart (PWM=0)
}


// posisjons-PID-steg: beregn u og send til motoren
void DC_motor::update_pid(long current_position, float dt_s) {
  // 1) Feil (ønsket - målt)
  long e = desired_position - current_position;

  //  Deadband for å hindre mikrojustering nær målposisjon
  const long DEADBAND_TICKS = 10;                 // område rundt ønsket posisjon der små feil ignoreres
  if (labs(e) < DEADBAND_TICKS) {
    e     = 0;                                    // behandle systemet som om det er i mål
    _e_int = 0.0f;                                // nullstill integrator for å hindre oppbygging nær målområdet
  }

  // 2) Derivert av feil (D-ledd)
  float edot = 0.0f;
  if (dt_s > 0.0f) {
    edot = float(e - _e_prev) / dt_s;             // (e[k] - e[k-1]) / dt
  }

  // 3) Integrert feil (I-ledd)
  _e_int += e * dt_s;

  // Anti-windup: hindrer at integratorverdien vokser ukontrollert
  const float I_MAX = 20000.0f;          // maksimal tillatt størrelse på integrator
  if (_e_int >  I_MAX) _e_int =  I_MAX;  // begrens positiv verdi
  if (_e_int < -I_MAX) _e_int = -I_MAX;  // begrens negativ verdi

  // 4) PID-utgang
  pid_signal = Kp * e + Ki * _e_int + Kd * edot;

  // 5) Mett PID-utgangen til området [-motor_speed_max, motor_speed_max]
  if (pid_signal >  motor_speed_max) pid_signal =  motor_speed_max;
  if (pid_signal < -motor_speed_max) pid_signal = -motor_speed_max;

  control_signal = pid_signal;            // lagres for logging/diagnose

  // 6) Send kontrollsignalet til motoren
  motor_control(control_signal);

  // 7) Lagre feil for neste tidssteg
  _e_prev = e;
}


// Kjør motor i valgt retning og hastighet basert på kontrollsignal
void DC_motor::motor_control(float control) {
  float u = control;

  // Sett retning og ta absoluttverdien av kontrollsignalet
  if (u >= 0.0f) {
    digitalWrite(_pin_phase, HIGH);                       // fremover
  } else {
    digitalWrite(_pin_phase, LOW);                        // bakover
    u = -u;                                               // bruk absoluttverdi for PWM
  }

  // Sikkerhet: klipp PWM-en til [0, motor_speed_max]
  if (u > motor_speed_max) {
    u = motor_speed_max;
  }

  analogWrite(_pin_enable, (int)u);                       // PWM-signal til driver
}

// Nødstopp/av: PWM=0 (retning beholdes uendret)
void DC_motor::stop() { 
  analogWrite(_pin_enable, 0);                            // motor av umiddelbart
}
