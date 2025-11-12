#include "Elevator.h"

// Oppretter ett globalt Elevator-objekt (systemet består av dette ene objektet)
Elevator elevator;

// Arduino-funksjonen setup() kjører én gang ved oppstart
void setup() { elevator.setup(); }// kaller Elevator::setup() → initialiserer alle delsystemer: "LCD, motorer, knapper, sensorer, osv."


// Arduino-funksjonen loop() kjører kontinuerlig etter setup()
void loop()  { elevator.loop();  } // kaller Elevator::loop() → utfører heislogikken: "leser knapper, oppdaterer motorer, dørstyring, nød, overvekt, osv.
