#pragma once // brukes slik at headerene ikke kan gjenntas mer enn 1 gang
#include <Arduino.h>
#include "Config.h"
#include "LCD.h"
#include "Overweight.h" 
#include "Queue.h"
#include "Buttons.h"
#include "Step_motor.h"
#include "Encoder.h"
#include "DC_motor.h"
#include "LEDs.h"

// Hovedklasse for hele heissystemet
class Elevator {
public:
  Elevator();                             // konstruktør – oppretter og initierer delsystemene
  void setup();                           // kalles én gang fra Arduino::setup() for å initialisere alt
  void loop();                            // hoved-kjøringen – kalles kontinuerlig fra Arduino::loop()

private:
  //Systemtilstand (variabler som beskriver hva heisen gjør nå) 
  int  _current_floor;                    // hvilken etasje heisen befinner seg i
  Dir  _direction;                        // nåværende retning: opp, ned eller idle
  bool _door_closed;                      // true hvis døren er helt lukket
  bool _servicing;                        // true når heisen er i ferd med å utføre en forespørsel

  //  Komponenter (hvert delsystem representert som et objekt) 
  LCD            _lcd;                    // displayet som viser etasje og statusmeldinger
  Overweight     _over;                   // sensor for overvekt
  Queue          _queue;                  // forespørselssystemet som bestemmer neste mål
  Buttons        _buttons;                // samler hall-, innvendige- og nødknappene
  Step_motor     _door;                   // motoren som styrer dørbevegelse
  Encoder        _enc;                    // leser motorposisjon (for DC-motoren)
  DC_motor       _dc;                     // DC-motor med PID-regulator (selve heisbevegelsen)
  LEDs           _leds;                   // LED-indikator for etasjenummer

  // Hjelpevariabler
  unsigned long  _prev_ms;                // lagrer siste tidsstempel for PID-oppdatering 

  // Interne hjelpefunksjoner 
  void arrive_at_floor(int f);            // kjører sekvens når heisen ankommer en etasje (stopper, åpner/lukker dør osv.)
  
  // konverterer etasje til posisjon i "encoder-ticks"
  long floor_to_ticks(int f) const { 
    return (long)((f - MIN_FLOOR) * TICKS_PER_FLOOR); 
  }

  // finner nærmeste etasje basert på nåværende posisjon i "ticks"
  int  ticks_to_floor(long ticks) const;  // definert i Elevator.cpp

  enum StatusMsg { ST_IDLE, ST_EMERGENCY, ST_OVERWEIGHT, ST_DOOR_OPEN, ST_DOOR_CLOSE, ST_MOVE, ST_NONE, ST_MOVE_UP, ST_MOVE_DOWN };
StatusMsg _lastStatus = ST_NONE;
unsigned long _lastStatusMs = 0;
void show_status(StatusMsg s, const char* txt, uint16_t min_interval_ms);

};
