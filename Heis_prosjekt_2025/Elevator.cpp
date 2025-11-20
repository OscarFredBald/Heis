#include "Elevator.h"                               
#include "dac.h"

// Konstruktør: initialiserer alle medlemmer i riktig rekkefølge
Elevator::Elevator()
: _current_floor(MIN_FLOOR),                           // start i nederste etasje
  _direction(DIR_IDLE),                                // start uten bevegelse
  _door_closed(true),                                  // antar dør lukket initialt
  _lcd(LCD_RS, LCD_E, LCD_D4, LCD_D5, LCD_D6, LCD_D7), // LCD-pinner
  _over(A_OVER, 700),                                  // overvekt-sensor på A0, terskel 700
  _queue(),                                            // tom forespørselkø
  _buttons(BTN_INSIDE, EMER_PIN),                      // innvendige knapper + nødknapp gjennom aggregator
  _door(ST_ENA, ST_PHA, ST_ENB, ST_PHB, 200),          // stepmotor for dør: EN/PHA/ENB/PHB, 200 steps
  _enc(ENC_A, ENC_B),                                  // enkoder A/B
  _dc(DC_EN, DC_PH, DC_DCY),                           // DC-motor pinner (enable/phase/decay)
  _leds(LED_FLOOR),                                    // etasje-LED-pinner
  _prev_ms(0),                                         // for dt-beregning
  _servicing(false)                                    // ikke i gang med en jobb enda
{}                                                     // tom konstruktørkropp

void Elevator::setup() {
  Serial.begin(115200);                                // serie for debug/tastatur
  dac_init();                                          // init DAC (hvis brukt i prosjektet)
  set_dac(2200, 2200);                                 // referanse til AIOSL

  _lcd.show_msg("Starter opp"); delay(500);            // status på LCD
  _door.opendoor(); delay(500);                        // kjapp dørtest
  _door.closedoor(); delay(500);

  _lcd.LCD_setup();                                    // init LCD (16x2) og vis “Heis klar”
  _lcd.show_msg("Init");

  _over.overweight_setup();                            // init overvekt-sensor (pinMode)
  _queue.queue_setup();                                // init kø
  _buttons.hall.hall_buttons_setup();                  // hall-knapper (PC-tastatur)
  _buttons.elev.setup();                               // innvendige knapper (INPUT_PULLUP)
  _door.step_motor_setup();                            // konfigurer stepmotorpinner
  _enc.encoder_setup();                                // sett opp ISR på enkoder A/B
  _dc.dc_motor_setup();                                // sett motorpinner, stopp motor
  _leds.LED_setup();                                   // klargjør etasje-LEDs

  //  PID-tuning

  _dc.motor_speed_max = 255;                           // PWM

  _leds.floor_indicator(_current_floor);               // vis start-etasje LED
  _lcd.show_floor(_current_floor);                     // vis start-etasje LCD
  _lcd.show_msg("Klar");                               // klar-status

  _buttons.emergency.emergency_setup();                // init nødknapp
}

void Elevator::show_status(StatusMsg s, const char* txt, uint16_t min_interval_ms) {
  unsigned long now = millis();
  // Oppdater displayet hvis statusen har endret seg,
  // eller hvis det har gått lenge nok siden sist oppdatering
  if (s != _lastStatus || (now - _lastStatusMs) > min_interval_ms) {
    _lcd.show_msg(txt);   // viser statusmeldingen på LCD uten å spamme displayet kontinuerlig
    _lastStatus = s;      // lagrer forrige status for neste sammenligning
    _lastStatusMs = now;  // tidsstempel for siste statusoppdatering
  }
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
  
  unsigned long now = millis();     //  felles "nå"-tidspunkt for denne loopen

  // 0) Telemetri til Serial Studio - kjører alltid, uansett om heisen står eller går
static unsigned long lastLogMs = 0;
const unsigned long LOG_DT_MS = 200;

if (now - lastLogMs >= LOG_DT_MS) { //  denne kjører alltid
  lastLogMs = now;

  long ticks = _enc.get_position();
  int  f_est = ticks_to_floor(ticks);

  Serial.print(_dc.desired_position);  Serial.print(',');
  Serial.print(ticks);                 Serial.print(',');
  Serial.print(f_est);                 Serial.print(',');
  Serial.println(_dc.control_signal);
}


  //Nødknapp
  _buttons.emergency.emergency_update();
if (_buttons.emergency.is_active()) {
  _dc.stop();
  show_status(ST_EMERGENCY, "EMERGENCY", 400);
  return;
}

  // 1) Innvendige knapper (edge-detect)
  int insideReq = _buttons.elev.read_pressed();           // -1 eller 1..4
  if (insideReq >= MIN_FLOOR && insideReq <= MAX_FLOOR) {
    _queue.add_inside(insideReq);                         // legg til inside request
    _servicing = true;                                    // vi har en jobb
    _lcd.show_msg("Inside req");                          // status
  }

  // 2) Hall-knapper via tastatur (implementert i Hall_buttons::hall_buttons)
  int hallReq = _buttons.hall.hall_buttons();   // 0 = ingen, >0 = opp, <0 = ned
if (hallReq != 0) {
  int floor = abs(hallReq);
  if (floor >= MIN_FLOOR && floor <= MAX_FLOOR) {
    if (hallReq > 0) {
      _queue.add_hall_up(floor);
    } else {
      _queue.add_hall_down(floor);
    }
    _servicing = true;
  }
}

  // 3) Overvekt-sjekk (sikkerhet)
  if (_over.overweight()) {
  _dc.stop();
  show_status(ST_OVERWEIGHT, "Overvekt!", 400);
  return;
}

  // 4) Finn target iht. REQ16 (opp/ned policy)

// Lokal lambda (funksjon) som regner ut "hvilken etasje vi befinner oss i"
// basert på encoderens posisjon (ticks) og gjeldende retning.
// Den gjør posisjonsavrunding retningstilpasset, slik at heisen ikke "snur"
// når noen kaller bak deg.
auto floor_from_ticks_dir = [&](long ticks, Dir dir) {
  double f_rel = double(ticks) / double(TICKS_PER_FLOOR);  // 0.0 = MIN_FLOOR, 1.0 = neste, ...
  double base  = MIN_FLOOR + f_rel;                        // faktisk etasjenivå i flyttall
  //  Retningsbasert avrunding:
  //  - Når vi går opp, tar vi "gulv" (nederste hele etasje under oss)
  //  - Når vi går ned, tar vi "tak" (øverste hele etasje over oss)
  int f = (dir == DIR_DOWN) ? int(ceil(base)) : int(floor(base));
  // Sikre at resultatet er innenfor gyldige etasjer
  if (f < MIN_FLOOR) f = MIN_FLOOR;
  if (f > MAX_FLOOR) f = MAX_FLOOR;
  return f;
};

// Beregn nåværende etasje basert på encoderposisjon og retning
int car_floor = floor_from_ticks_dir(_enc.get_position(), _direction);

// Finn neste mål ut fra faktisk posisjon og aktiv retning (REQ16 / SCAN)
int target = _queue.queue(car_floor, _direction);

// Hvis ingen forespørsel igjen, sett _servicing = false
if (target == -1 && !_queue.any_requests()) {
  _servicing = false;
}


  // 5) Tidssteg for PID 
  float dt = (_prev_ms == 0) ? DT_DEFAULT_S : (now - _prev_ms) / 1000.0f;
  _prev_ms = now;

  // 6) Ingen mål -> stå i ro
  if (target == -1) {
  _direction = DIR_IDLE;
  _dc.stop();

  static unsigned long lastIdleMsg = 0;
  if (millis() - lastIdleMsg > 300) {  // skriv “Idle” maks 3 ganger/sek
    _lcd.show_msg("Idle");
    lastIdleMsg = millis();
  }
  return;
}

  // 7) Bevegelse mellom etasjer (dør må være lukket for å kjøre)
  if (_door_closed) {
    _dc.desired_position = floor_to_ticks(target);         // ønsket posisjon (ticks)
    _dc.update_pid(_enc.get_position(), dt);               // PID-oppdatering mot settpunkt

    long err = _dc.desired_position - _enc.get_position(); // posisjonsfeil i ticks

    // Hvis vi er i service og innen toleranse -> vi er fremme
    if (_servicing && labs(err) < POS_TOLERANCE) {
      arrive_at_floor(target);                             // kjør dørsekvens + clear
      _servicing = _queue.any_requests();                  // er det mer å gjøre?
      // Retningsvalg håndteres i neste queue()-kall (SCAN holder/byter retning)
    }

    // Oppdater LCD med retning (enkelt statushint)
    if (_direction == DIR_UP)      show_status(ST_MOVE_UP,  "Opp", 300);
else if (_direction == DIR_DOWN) show_status(ST_MOVE_DOWN,"Ned", 300);
  } else {
    // Hvis døra sto åpen -> lukk før bevegelse
    _lcd.show_msg("Lukker dorer");
    _door.closedoor();
    _door_closed = true;
  }
}
