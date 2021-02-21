#pragma once

#include <iostream>

using byte = uint8_t;
using boolean = bool;

enum pin_level {LOW = 0, HIGH = 1};
enum pin_mode {INPUT, OUTPUT, INPUT_PULLUP};
enum bit_order {MSBFIRST, LSBFIRST};
constexpr int LED_BUILTIN = 13;
constexpr int A0 = 14;
constexpr int A1 = 15;
constexpr int A2 = 16;
constexpr int A3 = 17;
constexpr int A4 = 18;
constexpr int A5 = 19;

void pinMode();

void digitalWrite(int pin, pin_level value);
pin_level digiralRead(int pin);
void shiftOut(int dataPin, int clockPin, bit_order bitOrder, byte value);

void delay(unsigned long ms);
void delayMicroseconds(unsigned int us);
unsigned long millis();
unsigned long micros();

class Funshield_ {
	static constexpr pin_led[] = { 13,12,11,10 };
	static constexpr pin_button[] = { A1, A2, A3 };
	static constexpr latch_pin = 4;
	static constexpr clock_pin = 5;
	static constexpr data_pin = 8;
};

// Strings
#define isAlpha isalpha
#define isAlphaNumeric isalnum
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

// Okrajove pripady k vyreseni
// Cteni neexistujiciho pinu: dava LOW
// Cteni pinu jineho nez tlacitka: podle toho, co je na pinu na shiedlu pripojeno, nekdy zalezi i na tom, jestli se nahodou nedotknu nejakeho kontaktu
// Zapis pri nastavenem pinmode na input
// cteni pri nastavenem pinmode na output
// SEGM.DISPLEJ
//// - nenastaveny latch pred zapisem
//// - nenastaveny latch po zapisu
//// - shift pouze 1 byte misto dvou
//// - shift 3 nebo 4 byte
//// - LSBFIRST misto MSBFIRST