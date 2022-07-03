#ifndef VAR_H
#define VAR_H

#include <Arduino.h>
// Define the array of leds
unsigned color = 0xFFFFFF;
byte mode = 5;
byte music_mode = 3;
byte music_val = 0;

unsigned cmpt = 0;
bool sens = true;

// #define SSID "Livebox-5b58"
// #define PASS "EAEDD52EA9EF75A35644ACFEF2"

#define FIXED_MODE 0
#define FADER_MODE 1
#define CHENILLE_MODE 2
#define FLIC_MODE 4
#define MUSIC_MODE 5
#define STOP_MODE 0x0A

#endif