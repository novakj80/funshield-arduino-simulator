#pragma once

#include <iostream>

enum pin_level {LOW, HIGH};
enum pin_mode {INPUT, OUTPUT};

void pinMode();

void digitalWrite();
pin_level digiralRead(int pin);
void shiftOut();

void delay(unsigned long ms);
void delayMicroseconds(unsigned int us);
unsigned long millis();
unsigned long micros();

// Strings
#define isAlpha isalpha
#define isAlphaNumeric isalnum
// isAscii()
#define isControl iscntrl
#define isDigit isdigit
#define isGraph isgraph
#define isHexadecimalDigit isxdigit
#define isLowerCase islower
#define isPrintable isprint
#define isPunct ispunct
#define isSpace isspace
#define isUpperCase isupper
#define isWhitespace isblank