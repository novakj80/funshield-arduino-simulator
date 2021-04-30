#pragma once

#include <iostream>
#include <chrono>

using byte = uint8_t;

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

void pinMode(int pin, pin_mode mode);

void digitalWrite(int pin, pin_level value);
pin_level digitalRead(int pin);
void shiftOut(int dataPin, int clockPin, bit_order bitOrder, byte value);

void delay(unsigned long ms);
void delayMicroseconds(unsigned int us);
unsigned long millis();
unsigned long micros();

class Funshield_ {
public:
	void pinMode(int pin, pin_mode mode);
	void digitalWrite(int pin, pin_level value);
	pin_level digitalRead(int pin);
	void shiftOut(int dataPin, int clockPin, bit_order bitOrder, byte value);
	unsigned long millis();
	unsigned long micros();

	// Global funshield instance getter
	static Funshield_& getInstance();

	// Functions for getting funshield state
	bool isLedOn(int led);
	void setButton(int button, bool pressed);
	void resetTime() { start_time = std::chrono::high_resolution_clock::now(); }
	byte getGlyph(int pos);
	static constexpr int led_count = 4;
	static constexpr int button_count = 3;
	static constexpr int digit_count = 4;
	static constexpr byte segm_off = 0xFF;
private:
	static Funshield_ instance;
	//static constexpr int pin_led[] = { 13,12,11,10 };
	//static constexpr int pin_button[] = { A1, A2, A3 };

	static constexpr int first_led = 10;

	static constexpr int first_button = A1;

	static constexpr int latch = 4;
	static constexpr int clock = 7;
	static constexpr int data = 8;

	struct pin {
		pin_mode mode = pin_mode::INPUT;
		pin_level level = HIGH;
	};

	pin led_pin[led_count];
	pin button_pin[button_count];
	pin latch_pin;
	pin clock_pin;
	pin data_pin;

	// Segment display data are intentionaly uninitialized
#pragma warning(disable: 26495)
	byte segm_pos;
	byte segm_data;
	byte segm_pos_buffer;
	byte segm_data_buffer;
#pragma warning(default: 26495)

	std::chrono::time_point<std::chrono::high_resolution_clock> start_time;

	bool pinIsLed(int pin) { return pin >= first_led && pin < first_led + led_count; }
	bool pinIsButton(int pin) { return pin >= first_button && pin < first_button + button_count; }
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

class String
{
public:
	void toCharArray(char* buff, unsigned int len) {};
};

// Dummy implementation of serial monitor
// Allows programs which use serial monitor to compile without modifications
class serial_monitor
{
public:
	size_t available() { return 0; };
	size_t availableForWrite() { return 0; };
	void begin(int speed) {};
	template <typename T> size_t print(T val) { return 0; };
	template <typename T> size_t println(T val) { return 0; };
	String readString() { return {}; };

};

extern serial_monitor Serial;

// random
#include <cstdlib>
long random(long max);
long random(long min, long max);

void setup();
void loop();
// Okrajove pripady k vyreseni

// LED:
//// Cteni: pri input vzdy HIGH, pri output to, co bylo zapsano
//// pinMode INPUT: LED je zhaslá, Zapis pri nastavenem pinmode na input: nedělá nic a digitalRead precte HIGH. 
//// zmena pinmode na OUTPUT vzdy rozsviti LED (jakoby digitalWrite(LOW))

// Cteni pinu jineho: podle toho, co je na pinu na shiedlu pripojeno, nekdy zalezi i na tom, jestli se nahodou nedotknu nejakeho kontaktu
// Cteni neexistujiciho pinu: dava LOW

// SEGM.DISPLEJ
//// registr obsahuje shift registr a storage registr, data se přesunou ze shift do storage při přechodu LATCH z LOW na HIGH

// TODO list
// implementovat vsechny funkce z API
// -muzu zjednodusit navratove hodnoty digitalread