#include "DC_motor.h"


// Konstruktør: initialiserer pinner, standardverdier og PID-tilstander
DC_motor::DC_motor(uint8_t pin_enable, uint8_t pin_phase, uint8_t pin_decay)
: _pin_enable(pin_enable),              // lagre PWM-/ENABLE-pin for hastighetsstyring
  _pin_phase(pin_phase),                // lagre PHASE-pin for retningsstyring
  _pin_decay(pin_decay),                // lagre DECAY-pin (bremse-/freewheel-modus på driver)
  desired_position(0),                  // start-settpunkt (ønsket posisjon) = 0
  control_signal(0),                    // sist beregnede kontrollsignal (for logging)
  motor_speed_max(255/2),               // maks PWM (halv fart som standard for sikker oppstart)
  pid_signal(0),                        // PID-utgang u = 0 ved start
  Kp(1.0f), Ki(0.0f), Kd(0.0f),       // PID-parametere (kan tunes)
  _e_prev(0),                           // forrige feil e[k-1] = 0
  _e_int(0) {}                          // integrert feil (sum e*dt) = 0

// Oppsett av I/O-pinner og sikker starttilstand
void DC_motor::dc_motor_setup() {
  pinMode(_pin_enable, OUTPUT);         // ENABLE som utgang (PWM)
  pinMode(_pin_phase,  OUTPUT);         // PHASE som utgang (retning)
  pinMode(_pin_decay,  OUTPUT);         // DECAY som utgang (bremse-/freewheel-modus)
  digitalWrite(_pin_decay, LOW);        // sett DECAY lav (avhenger av driver: typisk “fast decay”/freewheel)
  analogWrite(_pin_enable, 0);          // motor av ved oppstart (PWM=0)
}

// posisjons-PID-steg: beregn u og send til motoren
void DC_motor::update_pid(long current_position, float dt_s) {
  long e = desired_position - current_position;         // posisjonsfeil: ønsket - målt
  float edot = (dt_s > 0) ? float(e - _e_prev)/dt_s     // deriverte feil: (e - e_prev)/dt
                          : 0.0f;                       // beskyttelse mot dt=0
  _e_int += e * dt_s;                                   // integrer feilen over tid (I-ledd)
  pid_signal = Kp*e + Ki*_e_int + Kd*edot;              // PID-utgang u = Kp*e + Ki*∫e + Kd*de/dt

 // if (pid_signal >  motor_speed_max) pid_signal =  motor_speed_max;  // mett på +maks PWM (pwm/2) som nevnt i h.fil
 // if (pid_signal < -motor_speed_max) pid_signal = -motor_speed_max;  // mett på -maks PWM (pwm/2)

  motor_control(pid_signal);                             // send kontrollsignalet til motoren
  _e_prev = e;                                           // lagre feil for neste tidssteg (til D-ledd)
}

// Kjør motor i valgt retning og hastighet basert på kontrollsignal
void DC_motor::motor_control(float control) {
  if (control > 0) {                                     // positiv u → kjør “fremover”
    digitalWrite(_pin_phase, HIGH);                      // sett retning fremover
    analogWrite(_pin_enable, (int)control);              // PWM = |u| (hastighet)
  } else {                                               // negativ u → kjør “bakover”
    digitalWrite(_pin_phase, LOW);                       // sett retning bakover
    analogWrite(_pin_enable, (int)(-control));           // PWM = |u|
  }
}

// Nødstopp/av: PWM=0 (retning beholdes uendret)
void DC_motor::stop() { 
  analogWrite(_pin_enable, 0);                           // motor av umiddelbart
}
