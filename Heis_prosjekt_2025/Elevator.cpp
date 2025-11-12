#include "Elevator.h"                               
#include "dac.h"

// Konstruktør: initialiserer alle medlemmer i riktig rekkefølge
Elevator::Elevator()
: _current_floor(MIN_FLOOR),                        // start i nederste etasje
  _direction(DIR_IDLE),                             // start uten bevegelse
  _door_closed(true),                               // antar dør lukket initialt
  _lcd(LCD_RS, LCD_E, LCD_D4, LCD_D5, LCD_D6, LCD_D7), // LCD-pinner
  _over(A_OVER, 700),                               // overvekt-sensor på A0, terskel 700
  _queue(),                                         // tom forespørselkø
  _buttons(BTN_INSIDE, EMER_PIN),                   // innvendige knapper + nødknapp gjennom aggregator
  _door(ST_ENA, ST_PHA, ST_ENB, ST_PHB, 200),       // stepmotor for dør: EN/PHA/ENB/PHB, 200 steps
  _enc(ENC_A, ENC_B),                               // enkoder A/B
  _dc(DC_EN, DC_PH, DC_DCY),                        // DC-motor pinner (enable/phase/decay)
  _leds(LED_FLOOR),                                 // etasje-LED-pinner
  _prev_ms(0),                                      // for dt-beregning
  _servicing(false)                                 // ikke i gang med en jobb enda
{}                                                  // tom konstruktørkropp

void Elevator::setup() {
  Serial.begin(115200);                             // serie for debug/tastatur
  dac_init();                                       // init DAC (hvis brukt i prosjektet)
  set_dac(2200, 2200);                              // sett startnivå (0..4095), myk start

  _lcd.show_msg("Starter opp");                     // status på LCD
  _door.opendoor(); delay(500);                     // kjapp dørsekvens for å verifisere stepmotor
  _door.closedoor(); delay(500);

  _lcd.LCD_setup();                                 // init LCD (16x2) og vis “Heis klar”
  _lcd.show_msg("Init");

  _over.overweight_setup();                         // init overvekt-sensor (pinMode)
  _queue.queue_setup();                             // evt. fremtidig init (tom nå)
  _buttons.hall.hall_buttons_setup();               // hall-knapper (nå: tastatur via Serial)
  _buttons.elev.setup();                            // innvendige knapper (INPUT_PULLUP)
  _door.step_motor_setup();                         // konfigurer stepmotorpinner
  _enc.encoder_setup();                             // sett opp ISR på enkoder A/B
  _dc.dc_motor_setup();                             // sett motorpinner, stopp motor
  _leds.LED_setup();                                // klargjør etasje-LEDs

  _leds.floor_indicator(_current_floor);            // vis start-etasje LED
  _lcd.show_floor(_current_floor);                  // vis start-etasje LCD
  _lcd.show_msg("Klar");                            // klar-status

  _buttons.emergency.emergency_setup();             // init nødknapp (INPUT_PULLUP + tilstander)
}

// konverter encoder-ticks til nærmeste etasje (klamrer til [MIN_FLOOR, MAX_FLOOR])
int Elevator::ticks_to_floor(long ticks) const {
  int f = MIN_FLOOR + (int)lround( (double)ticks / (double)TICKS_PER_FLOOR ); // nærmeste
  if (f < MIN_FLOOR) f = MIN_FLOOR;                                           // klamp nedre (min grense)
  if (f > MAX_FLOOR) f = MAX_FLOOR;                                           // klamp øvre (max grense)
  return f;
}

// Håndterer ankomst: stopp, oppdater status, åpne/lukk dører, clear requests
void Elevator::arrive_at_floor(int f) {
  _dc.stop();                                         // stopp DC-motor
  _current_floor = f;                                 // oppdater etasje
  _leds.floor_indicator(f);                           // sett LED for etasje
  _lcd.show_floor(f);                                 // vis etasje på LCD

  delay(1000);                                        // liten sikkerhetsvent
  _lcd.show_msg("Apner dorer");                       // status: åpne
  _door.opendoor();                                   // åpne dør
  _door_closed = false;                               // dør er åpen
  _lcd.show_msg("Dorene er apne");                    // status

  _queue.clear_at(f);                                 // fjern alle forespørsler for denne etasjen (REQ13)

  delay(2000);                                        // hold døra åpen litt
  _lcd.show_msg("Lukker dorer");                      // status: lukker
  _door.closedoor();                                  // lukk dør
  _door_closed = true;                                // dør er lukket
  _lcd.show_msg("Dorene er lukket");                  // status
  delay(1000);                                        // liten pause før ny bevegelse
}


void Elevator::loop() {
  _buttons.emergency.emergency_update();
  if (_buttons.emergency.is_active()) {               // hvis nød aktiv → stopp alt
    _dc.stop();
    _lcd.show_msg("EMERGENCY");
    return;
  }

  // 1) Innvendige knapper (edge-detect)
  int insideReq = _buttons.elev.read_pressed();       // -1 eller 1..4
  if (insideReq >= MIN_FLOOR && insideReq <= MAX_FLOOR) {
    _queue.add_inside(insideReq);                     // legg til inside request
    _servicing = true;                                // vi har en jobb
    _lcd.show_msg("Inside req");                      // status
  }

  // 2) Hall-knapper via tastatur (implementert i Hall_buttons::hall_buttons)
  int hallReq = _buttons.hall.hall_buttons();         // -1 eller 1..4
  if (hallReq >= MIN_FLOOR && hallReq <= MAX_FLOOR) {
    if (hallReq > _current_floor)      _queue.add_hall_up(hallReq);   // opp-ønske
    else if (hallReq < _current_floor) _queue.add_hall_down(hallReq); // ned-ønske
    else                               _queue.add_inside(hallReq);    // samme etasje → inside
    _servicing = true;
    _lcd.show_msg("Hall req");                          // status
  }

  // 3) Overvekt-sjekk (sikkerhet)
  if (_over.overweight()) {
    _dc.stop();
    _lcd.show_msg("Overvekt!");
    return;                                             // ikke kjør før last er trygg
  }

  // 4) Finn neste target iht. SCAN-policy (REQ16)
  int target = _queue.queue(_current_floor, _direction);
  if (target == -1 && !_queue.any_requests()) {
    _servicing = false;                                 // ingenting å gjøre
  }

  // 5) Tidssteg for PID 
  unsigned long now = millis();
  float dt = (_prev_ms == 0) ? DT_DEFAULT_S : (now - _prev_ms) / 1000.0f;
  _prev_ms = now;

  // 6) Ingen mål → stå i ro
  if (target == -1) {
    _direction = DIR_IDLE;
    _dc.stop();
    _lcd.show_msg("Idle");
    return;
  }

  // 7) Bevegelse mellom etasjer (dør må være lukket for å kjøre)
  if (_door_closed) {
    _dc.desired_position = floor_to_ticks(target);      // ønsket posisjon (ticks)
    _dc.update_pid(_enc.get_position(), dt);            // PID-oppdatering mot settpunkt

    long err = _dc.desired_position - _enc.get_position(); // posisjonsfeil i ticks

    // Hvis vi er i service og innen toleranse → vi er fremme
    if (_servicing && labs(err) < POS_TOLERANCE) {
      arrive_at_floor(target);                          // kjør dørsekvens + clear
      _servicing = _queue.any_requests();               // er det mer å gjøre?
      // Retningsvalg håndteres i neste queue()-kall (SCAN holder/byter retning)
    }

    // Oppdater LCD med retning (enkelt statushint)
    if (_direction == DIR_UP)       _lcd.show_msg("Opp");
    else if (_direction == DIR_DOWN) _lcd.show_msg("Ned");
  } else {
    // Hvis døra sto åpen → lukk før bevegelse
    _lcd.show_msg("Lukker dorer");
    _door.closedoor();
    _door_closed = true;
  }
}
