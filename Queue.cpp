#include "Queue.h"

Queue::Queue() : target_floor(-1) {
  for (int f=MIN_FLOOR; f<=MAX_FLOOR; ++f) {
    inside[idx(f)] = hall_up[idx(f)] = hall_down[idx(f)] = false;
  }
}
void Queue::queue_setup() {}

void Queue::add_inside(int floor)   { if (floor>=MIN_FLOOR && floor<=MAX_FLOOR) inside[idx(floor)] = true; }
void Queue::add_hall_up(int floor)  { if (floor>=MIN_FLOOR && floor< MAX_FLOOR) hall_up[idx(floor)]  = true; }
void Queue::add_hall_down(int floor){ if (floor> MIN_FLOOR && floor<=MAX_FLOOR) hall_down[idx(floor)] = true; }

void Queue::clear_at(int floor) {
  if (floor<MIN_FLOOR || floor>MAX_FLOOR) return;
  inside[idx(floor)] = false;
  hall_up[idx(floor)] = false;
  hall_down[idx(floor)] = false;
}

bool Queue::any_above(int floor) const {
  for (int f=floor+1; f<=MAX_FLOOR; ++f) {
    if (inside[idx(f)] || hall_up[idx(f)] || hall_down[idx(f)]) return true;
  }
  return false;
}
bool Queue::any_below(int floor) const {
  for (int f=floor-1; f>=MIN_FLOOR; --f) {
    if (inside[idx(f)] || hall_up[idx(f)] || hall_down[idx(f)]) return true;
  }
  return false;
}
bool Queue::any_requests() const {
  for (int f=MIN_FLOOR; f<=MAX_FLOOR; ++f) {
    if (inside[idx(f)] || hall_up[idx(f)] || hall_down[idx(f)]) return true;
  }
  return false;
}

int Queue::pick_in_direction(int current_floor, Dir dir) const {
  if (dir == DIR_UP) {
    for (int f=current_floor+1; f<=MAX_FLOOR; ++f) {
      if (inside[idx(f)] || hall_up[idx(f)] || hall_down[idx(f)]) return f;
    }
  } else if (dir == DIR_DOWN) {
    for (int f=current_floor-1; f>=MIN_FLOOR; --f) {
      if (inside[idx(f)] || hall_up[idx(f)] || hall_down[idx(f)]) return f;
    }
  }
  return -1;
}

// Kjerne (REQ16): behold retning så lenge det finnes forespørsler i den retningen.
// Bytt kun retning hvis det ikke finnes flere forespørsler i nåværende retning,
// men det finnes i motsatt retning.
int Queue::queue(int current_floor, Dir &direction) {
  if (!any_requests()) { target_floor = -1; direction = DIR_IDLE; return -1; }

  // Hvis vi står i idle: velg nærmeste forespørsel og sett retning
  if (direction == DIR_IDLE) {
    // Finn nærmeste etasje med forespørsel (enkel variant: let oppover først, deretter nedover)
    for (int radius=0; radius<=MAX_FLOOR-MIN_FLOOR; ++radius) {
      int up = current_floor + radius;
      int dn = current_floor - radius;
      if (up>=MIN_FLOOR && up<=MAX_FLOOR) {
        if (inside[idx(up)] || hall_up[idx(up)] || hall_down[idx(up)]) {
          direction = (up > current_floor) ? DIR_UP : (up < current_floor ? DIR_DOWN : DIR_IDLE);
          target_floor = up;
          return target_floor;
        }
      }
      if (radius>0 && dn>=MIN_FLOOR && dn<=MAX_FLOOR) {
        if (inside[idx(dn)] || hall_up[idx(dn)] || hall_down[idx(dn)]) {
          direction = (dn > current_floor) ? DIR_UP : (dn < current_floor ? DIR_DOWN : DIR_IDLE);
          target_floor = dn;
          return target_floor;
        }
      }
    }
  }

  // Forsøk å fortsette i nåværende retning
  int next_in_dir = pick_in_direction(current_floor, direction);
  if (next_in_dir != -1) { target_floor = next_in_dir; return target_floor; }

  // Ingen i samme retning -> bytt retning hvis det finnes noen der
  Dir other = (direction == DIR_UP) ? DIR_DOWN : DIR_UP;
  int next_other = pick_in_direction(current_floor, other);
  if (next_other != -1) { direction = other; target_floor = next_other; return target_floor; }

  // Skulle ikke skje (det finnes forespørsler), men safe:
  target_floor = -1; direction = DIR_IDLE; return -1;
}
