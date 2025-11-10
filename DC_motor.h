#pragma once
#include <Arduino.h>

class DC_motor {
public:
  DC_motor(uint8_t pin_enable, uint8_t pin_phase, uint8_t pin_decay);
  void dc_motor_setup();
  void update_pid(long current_position, float dt_s);
  void motor_control(float control);
  void stop();

  long  desired_position;
  float control_signal;      // ikke nødvendig, men beholdt
  int   motor_speed_max;     // 0..255
  float pid_signal;          // "u"

  float Kp, Ki, Kd;

private:
  uint8_t _pin_enable, _pin_phase, _pin_decay;
  long  _e_prev;
  float _e_int;
};
