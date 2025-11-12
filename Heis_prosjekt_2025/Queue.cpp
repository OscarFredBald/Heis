#include "Queue.h"

// Konstruktør: sett target til "ingen" 
Queue::Queue() : target_floor(-1) { // Setter alle forespørsler til "false" (ingen knapper trykket) : target_floor(-1) → heisen har ikke noe mål ved start
  for (int f=MIN_FLOOR; f<=MAX_FLOOR; ++f) { 
    inside[idx(f)] = hall_up[idx(f)] = hall_down[idx(f)] = false; // inside/hall_up/hall_down er false ved start (ingen knapper trykket)
  }
}
void Queue::queue_setup() {} // Tom init-funksjon for kompletthet (kan brukes hvis du vil re-initialisere senere)

void Queue::add_inside(int floor)   { if (floor>=MIN_FLOOR && floor<=MAX_FLOOR) inside[idx(floor)] = true; } // Registrer innside-forespørsel (gyldig i hele intervallet)
void Queue::add_hall_up(int floor)  { if (floor>=MIN_FLOOR && floor< MAX_FLOOR) hall_up[idx(floor)]  = true; }  // Registrer hall-opp (ikke tillatt i øverste etasje)
void Queue::add_hall_down(int floor){ if (floor> MIN_FLOOR && floor<=MAX_FLOOR) hall_down[idx(floor)] = true; } // Registrer hall-ned (ikke tillatt i nederste etasje)

// Fjern alle forespørsler i en etasje (kalles når dørene er helt åpne, REQ13)
void Queue::clear_at(int floor) {
  if (floor<MIN_FLOOR || floor>MAX_FLOOR) return; // Ugyldig etasje → ingenting
  // Nullstill alle tre kildene for denne etasjen
  inside[idx(floor)] = false;
  hall_up[idx(floor)] = false;  
  hall_down[idx(floor)] = false;
}

// Sjekk om det finnes *noen* forespørsler over current 'floor'
bool Queue::any_above(int floor) const {
  for (int f=floor+1; f<=MAX_FLOOR; ++f) {
    if (inside[idx(f)] || hall_up[idx(f)] || hall_down[idx(f)]) return true;
  }
  return false; // Ingen over
}

// Sjekk om det finnes *noen* forespørsler under current 'floor'
bool Queue::any_below(int floor) const {
  for (int f=floor-1; f>=MIN_FLOOR; --f) {
    if (inside[idx(f)] || hall_up[idx(f)] || hall_down[idx(f)]) return true;
  }
  return false; // Ingen under
}

// Sjekk om det finnes *noen* forespørsler i hele bygget
bool Queue::any_requests() const {
  for (int f=MIN_FLOOR; f<=MAX_FLOOR; ++f) {
    if (inside[idx(f)] || hall_up[idx(f)] || hall_down[idx(f)]) return true;
  }
  return false; // Ikke en eneste forespørsel
}


// Hjelper: Velg neste forespørsel i gitt retning (nærmest først)
// Returnerer -1 hvis ingen i den retningen.
int Queue::pick_in_direction(int current_floor, Dir dir) const {
  if (dir == DIR_UP) {          // Let fra etasjen over og opp til taket
    for (int f=current_floor+1; f<=MAX_FLOOR; ++f) {
      if (inside[idx(f)] || hall_up[idx(f)] || hall_down[idx(f)]) return f;
    }
  } else if (dir == DIR_DOWN) { // Let fra etasjen under og ned til kjelleren
    for (int f=current_floor-1; f>=MIN_FLOOR; --f) {
      if (inside[idx(f)] || hall_up[idx(f)] || hall_down[idx(f)]) return f;
    }
  }
  return -1; // Ingenting funnet i denne retningen
}

// Kjerne (REQ16): Hold retning så lenge det finnes forespørsler i den retningen.
// Bytt retning bare hvis tomt i nåværende retning, men noe finnes i motsatt.
// Returner -1 hvis ingenting å gjøre.
int Queue::queue(int current_floor, Dir &direction) {
  if (!any_requests()) { target_floor = -1; // Ikke noe mål
   direction = DIR_IDLE;                    // Stå stille
   return -1; } 

    // Hvis vi står stille, velg nærmeste forespørsel og sett retning mot den
  if (direction == DIR_IDLE) {
    // Finn nærmeste etasje med forespørsel (enkel variant: let oppover først, deretter nedover)
    for (int radius=0; radius<=MAX_FLOOR-MIN_FLOOR; ++radius) { // Radius-søk: sjekk samme etasje, så 1 opp/ned, 2 opp/ned, ...
      int up = current_floor + radius; // Kandidat oppover (inkl. samme etasje når radius=0)
      int dn = current_floor - radius; // Kandidat nedover

      //Sjekk oppover først (inkl. radius 0 = samme etasje)
      if (up>=MIN_FLOOR && up<=MAX_FLOOR) {
        if (inside[idx(up)] || hall_up[idx(up)] || hall_down[idx(up)]) {
          // Sett retning ut fra relasjon til current_floor
          direction = (up > current_floor) ? DIR_UP 
          : (up < current_floor ? DIR_DOWN : DIR_IDLE);
          target_floor = up;     // Husk valgt mål
          return target_floor;  // Returner målet
        }
      }
       // Sjekk nedover (hopp over radius 0 for å unngå duplikat)
      if (radius>0 && dn>=MIN_FLOOR && dn<=MAX_FLOOR) {
        if (inside[idx(dn)] || hall_up[idx(dn)] || hall_down[idx(dn)]) {
          direction = (dn > current_floor) ? DIR_UP : (dn < current_floor ? DIR_DOWN : DIR_IDLE);
          target_floor = dn; 
          return target_floor;
        }
      }
    }
  }

  // Hvis vi allerede har retning, prøv å fortsette i samme retning (SCAN)
  int next_in_dir = pick_in_direction(current_floor, direction);
  if (next_in_dir != -1) { 
    target_floor = next_in_dir; return target_floor; // Hold retning
    }

  // Tomt i denne retningen → prøv motsatt retning
  Dir other = (direction == DIR_UP) ? DIR_DOWN : DIR_UP;
  int next_other = pick_in_direction(current_floor, other);
  if (next_other != -1) {
     direction = other;           // Bytt retning
     target_floor = next_other;  // Sett nytt mål
      return target_floor; 
      }


}
