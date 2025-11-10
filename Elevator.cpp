#include "Elevator.h"
#include "dac.h"
#include "Config.h"


Elevator::Elevator()
: _current_floor(MIN_FLOOR),
  _direction(DIR_IDLE),
  _door_closed(true),
  _lcd(LCD_RS, LCD_E, LCD_D4, LCD_D5, LCD_D6, LCD_D7),
  _over(A_OVER, 700),
  _queue(),
  _buttons(BTN_INSIDE),
  _door(ST_ENA, ST_PHA, ST_ENB, ST_PHB, 200),
  _enc(ENC_A, ENC_B),
  _dc(DC_EN, DC_PH, DC_DCY),
  _leds(LED_FLOOR),
  _prev_ms(0),
  _servicing(false),
  _emergency(EMER_PIN)

{}

void Elevator::setup() {
  Serial.begin(115200);
  dac_init(); //konfigurer D30..D33 til MCP4922
  set_dac(2200, 2200);// 0..4095 start moderat (ca. midt på)
  _lcd.show_msg("Starter opp");
  _door.opendoor(); delay(500);
  _door.closedoor(); delay(500);
  _lcd.LCD_setup();
  _lcd.show_msg("Init");
  _over.overweight_setup();
  _queue.queue_setup();
  _buttons.hall.hall_buttons_setup();
  _buttons.elev.setup();
  _door.step_motor_setup();
  _enc.encoder_setup();
  _dc.dc_motor_setup();
  _leds.LED_setup();

  _leds.floor_indicator(_current_floor);
  _lcd.show_floor(_current_floor);
  _lcd.show_msg("Klar");

  _emergency.emergency_setup();

}

int Elevator::ticks_to_floor(long ticks) const {
  // nærmeste etasje (enkel mapping)
  int f = MIN_FLOOR + (int)lround( (double)ticks / (double)TICKS_PER_FLOOR );
  if (f < MIN_FLOOR) f = MIN_FLOOR;
  if (f > MAX_FLOOR) f = MAX_FLOOR;
  return f;
}

void Elevator::arrive_at_floor(int f) {
  _dc.stop();
  _current_floor = f;
  _leds.floor_indicator(f);
  _lcd.show_floor(f);

  // Dørsekvens (REQ12/13)
   delay(1000); //delay for å være sikker på at motor stoppet
  _lcd.show_msg("Apner dorer");
  _door.opendoor();
  _door_closed = false;
 _lcd.show_msg("Dorene er apne");
  // forespørsler slettes når dør er helt åpen (REQ13)
  _queue.clear_at(f); 

  delay(2000); // kort vent
  _lcd.show_msg("Lukker dorer");
  _door.closedoor();
  _door_closed = true;
  _lcd.show_msg("Dorene er lukket");
  delay(1000);
}

static int parse_floor_char(char ch) {
  if (ch >= '0' && ch <= '9') return ch - '0';
  return -1;//For å gi tastaturet som hall buttons //funnket
}


void Elevator::loop() {

_emergency.emergency_update();
if (_emergency.is_active()) {
  _dc.stop();           // stopp DC-motor
  _lcd.show_msg("EMERGENCY");
  return;
}

  // 1) Les knapper (inne)
  int insideReq = _buttons.elev.read_pressed();
  if (insideReq >= MIN_FLOOR && insideReq <= MAX_FLOOR) {
    _queue.add_inside(insideReq);
    _servicing = true; 
    _lcd.show_msg("Inside req");
  }
  // 2) Hall-knapper (du kan implementere faktisk pins for opp/ned pr etasje)
  int hallReq = _buttons.hall.hall_buttons(); // stub: -1
  if (hallReq >= MIN_FLOOR && hallReq <= MAX_FLOOR) {
    // hvis du vil, velg opp/ned basert på nåværende etasje
    if (hallReq > _current_floor) _queue.add_hall_up(hallReq);
    else if (hallReq < _current_floor) _queue.add_hall_down(hallReq);
    else _queue.add_inside(hallReq);
    _lcd.show_msg("Hall req");
  }

  // --- Tastatur "2u"/"3d" som hall-knapper --- //FUNKET
static int pendingFloor = -1;
while (Serial.available() > 0) {
  char c = tolower(Serial.read());
  int f = parse_floor_char(c);
  if (f != -1) {
    pendingFloor = f;                 // husk etasje
  } else if ((c == 'u' || c == 'd') && pendingFloor != -1) {
    if (pendingFloor >= MIN_FLOOR && pendingFloor <= MAX_FLOOR) {
      if (c == 'u' && pendingFloor < MAX_FLOOR) {
        _queue.add_hall_up(pendingFloor);
        _lcd.show_msg("Hall Up");
      } else if (c == 'd' && pendingFloor > MIN_FLOOR) {
        _queue.add_hall_down(pendingFloor);
        _lcd.show_msg("Hall Down");
      }
      _servicing = true;
    }
    pendingFloor = -1;                // reset
  } else {
    // ignorer andre tegn / evt. reset ved Enter
  }
}


  // 3) Overvekt? (REQ8 implisitt sikkerhet)
  if (_over.overweight()) {
    _dc.stop();
    _lcd.show_msg("Overvekt!");
    return;
  }

  // 4) Finn target iht. REQ16 (opp/ned policy)
  int target = _queue.queue(_current_floor, _direction);
  if (target == -1 && !_queue.any_requests()) {
  _servicing = false;
}

  // Tidssteg
  unsigned long now = millis();
  float dt = (_prev_ms==0) ? DT_DEFAULT_S : (now - _prev_ms)/1000.0f;
  _prev_ms = now;

  if (target == -1) {
    // REQ15: stå stille hvis ingen forespørsel
    _direction = DIR_IDLE;
    _dc.stop();
    _lcd.show_msg("Idle");
    return;
  }

  // 5) Bevegelse mellom etasjer (REQ1,3,4-6,8)
  if (_door_closed) {
    // sett ønsket posisjon
    _dc.desired_position = floor_to_ticks(target);
    _dc.update_pid(_enc.get_position(), dt);

    long err = _dc.desired_position - _enc.get_position();


   if (_servicing && labs(err) < POS_TOLERANCE) {
  arrive_at_floor(target);
  _servicing = _queue.any_requests();  // fortsatt noe igjen?
      // Etter ankomst, behold retning hvis det finnes forespørsler i samme retning,
      // ellers byttes retning i neste queue()-kall (REQ16)
   }
    // Oppdatér LCD med retning
    if (_direction == DIR_UP)      _lcd.show_msg("Opp");
    else if (_direction == DIR_DOWN) _lcd.show_msg("Ned");
  } else {
    // Dør åpen -> lukk før bevegelse (REQ8: dør lukket under bevegelse)
    _lcd.show_msg("Lukker dorer");
    _door.closedoor();
    _door_closed = true;
  }
}
