#pragma once
#include <Arduino.h>

// ---- Etasjeoppsett ----
// Bruker 1..4 som etasjer (endre til 0..3 ved behov)
static const int MIN_FLOOR = 1;
static const int MAX_FLOOR = 4;
static const long TICKS_PER_FLOOR = 15000; // juster etter rigg, justerer høydeforskjell
static const long POS_TOLERANCE   = 200;  // posisjonstoleranse
static const float DT_DEFAULT_S   = 0.02f;

// ---- Pins (oppdater om din rigg er annerledes) ----
static const uint8_t BTN_INSIDE[4] = {22, 23, 24, 25}; // 1..4
static const uint8_t LED_FLOOR[4]  = {49, 48, 47, 46}; // 1..4

static const uint8_t ENC_A = 20;
static const uint8_t ENC_B = 21;

static const uint8_t DC_EN = 7;
static const uint8_t DC_PH = 6;
static const uint8_t DC_DCY= 5;

static const uint8_t ST_ENA= 69;
static const uint8_t ST_PHA= 68;
static const uint8_t ST_ENB= 67;
static const uint8_t ST_PHB= 66;

static const uint8_t LCD_RS= 41;
static const uint8_t LCD_E = 40;
static const uint8_t LCD_D4= 37;
static const uint8_t LCD_D5= 36;
static const uint8_t LCD_D6= 35;
static const uint8_t LCD_D7= 34;

static const uint8_t A_OVER= A0;

// ---- Små hjelpetyper ----
enum Dir {DIR_UP=+1, DIR_DOWN=-1, DIR_IDLE=0};

// Emergency-knapp (default 26 for å unngå konflikt med DC_EN=7)
// Hvis du vil bruke 7 som knapp: flytt DC_EN til f.eks. 8 i dette prosjektet.
static const uint8_t EMER_PIN = 26; //KNAPP 4 på servolabben

