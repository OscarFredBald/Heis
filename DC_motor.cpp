#include "DC_motor.h"

DC_motor::DC_motor(uint8_t pin_enable, uint8_t pin_phase, uint8_t pin_decay)
: _pin_enable(pin_enable), _pin_phase(pin_phase), _pin_decay(pin_decay),
  desired_position(0), control_signal(0), motor_speed_max(255/2), pid_signal(0),
  Kp(0.5f), Ki(0.0f), Kd(0.005f), _e_prev(0), _e_int(0) {}

void DC_motor::dc_motor_setup() {
  pinMode(_pin_enable, OUTPUT);
  pinMode(_pin_phase,  OUTPUT);
  pinMode(_pin_decay,  OUTPUT);
  digitalWrite(_pin_decay, LOW);
  analogWrite(_pin_enable, 0);
}

void DC_motor::update_pid(long current_position, float dt_s) {
  long e = desired_position - current_position;
  float edot = (dt_s > 0) ? float(e - _e_prev)/dt_s : 0.0f;
  _e_int += e * dt_s;
  pid_signal = Kp*e + Ki*_e_int + Kd*edot;

  if (pid_signal >  motor_speed_max) pid_signal =  motor_speed_max;
  if (pid_signal < -motor_speed_max) pid_signal = -motor_speed_max;

  motor_control(pid_signal);
  _e_prev = e;
}

void DC_motor::motor_control(float control) {
  if (control > 0) {
    digitalWrite(_pin_phase, HIGH);
    analogWrite(_pin_enable, (int)control);
  } else {
    digitalWrite(_pin_phase, LOW);
    analogWrite(_pin_enable, (int)(-control));
  }
}
void DC_motor::stop() { analogWrite(_pin_enable, 0); }
