#pragma once
#include <Arduino.h>

// Etasjeområdet i anlegget (bruker etasjer 1..4)
static const int MIN_FLOOR = 1;                 // Laveste etasje-nummer
static const int MAX_FLOOR = 4;                 // Høyeste etasje-nummer

// Konvertering mellom encoder-ticks og fysisk etasjehøyde
static const long TICKS_PER_FLOOR = 15000;      // Antall enkoder-tikk per etasje 
static const long POS_TOLERANCE   = 200;        // Tillatt posisjonsavvik (± tikk) når vi sier at vi "er fremme"
static const float DT_DEFAULT_S   = 0.02f;      // tidssteg i sekunder for reguleringssløyfer (f.eks. PID)

// Pin-mapping for innvendige etasjeknapper (fire knapper for 1..4)
static const uint8_t BTN_INSIDE[4] = {22, 23, 24, 25};  // Digitalpinner koblet til etasjeknappene i heisen

// Pin-mapping for etasje-LEDs (viser hvilken etasje som er aktiv)
static const uint8_t LED_FLOOR[4]  = {49, 48, 47, 46};  // Én LED per etasje 1..4

// Enkoderpinner (kvadratur A/B) for posisjonsmåling
static const uint8_t ENC_A = 20;               // Enkoder kanal A (må være interrupt-kapabel pin)
static const uint8_t ENC_B = 21;               // Enkoder kanal B (må være interrupt-kapabel pin)

// DC-motor driverpinner
static const uint8_t DC_EN = 7;                // ENABLE (PWM) til DC-motordriver (hastighet)
static const uint8_t DC_PH = 6;                // PHASE/retning til DC-motordriver (retning)
static const uint8_t DC_DCY= 5;                // DECAY/BRAKE-pin (styrer brems/ freewheel ved driver som støtter det)

// Stepmotor driverpinner (to spoler A og B, hver med enable + phase)
static const uint8_t ST_ENA= 69;               // Enable for step-spole A (må være gyldig pin for din driver)
static const uint8_t ST_PHA= 68;               // Phase (retning) for spole A
static const uint8_t ST_ENB= 67;               // Enable for step-spole B
static const uint8_t ST_PHB= 66;               // Phase (retning) for spole B

// LCD-tilkobling (4-bit modus via LiquidCrystal)
static const uint8_t LCD_RS= 41;               // RS (Register Select)
static const uint8_t LCD_E = 40;               // E (Enable)
static const uint8_t LCD_D4= 37;               // Databit D4
static const uint8_t LCD_D5= 36;               // Databit D5
static const uint8_t LCD_D6= 35;               // Databit D6
static const uint8_t LCD_D7= 34;               // Databit D7

// Analog inngang for overlastsensor (potensiometer/vektføler)
static const uint8_t A_OVER= A0;               // Analog pin som måles med analogRead()

// Retningsenum for heislogikken
enum Dir {DIR_UP=+1, DIR_DOWN=-1, DIR_IDLE=0}; // +1 = opp, -1 = ned, 0 = står i ro


static const uint8_t EMER_PIN = 26;            // Digital pin for emergency-knappen (på servolabben: knapp 4)
