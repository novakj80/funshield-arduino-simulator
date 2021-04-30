#include "FunShield.h"

#include <thread>
Funshield_ Funshield_::instance{};
Funshield_& Funshield_::getInstance()
{
	return Funshield_::instance;
}
serial_monitor Serial;
void pinMode(int pin, pin_mode mode)
{
	Funshield_::getInstance().pinMode(pin, mode);
}

void digitalWrite(int pin, pin_level value) 
{
	Funshield_::getInstance().digitalWrite(pin, value);
}
pin_level digitalRead(int pin) 
{ 
	return Funshield_::getInstance().digitalRead(pin);
}
void shiftOut(int dataPin, int clockPin, bit_order bitOrder, byte value) { Funshield_::getInstance().shiftOut(dataPin, clockPin, bitOrder, value); }
unsigned long millis() { return Funshield_::getInstance().millis(); }
unsigned long micros() { return Funshield_::getInstance().micros(); }
void Funshield_::pinMode(int pin, pin_mode mode)
{
	// LED
	if (pinIsLed(pin)) {
		int i = pin - first_led;
		led_pin[i].mode = mode;
		led_pin[i].level = mode == OUTPUT ? LOW : HIGH;
	}
	// BUTTON
	else if (pinIsButton(pin)) {
		button_pin[pin - first_button].mode = mode;
	}
	// SEGMENT DISPLAY
	else switch (pin) {
	case latch:
		latch_pin.mode = mode; break;
	case clock:
		clock_pin.mode = mode; break;
	case data:
		data_pin.mode = mode; break;
	}
}

pin_level Funshield_::digitalRead(int pin)
{
	if (pinIsButton(pin)) {
		return button_pin[pin - first_button].level;
	}

	if (pinIsLed(pin)) {
		if (led_pin[pin - first_led].mode != OUTPUT) return HIGH;
		else return led_pin[pin - first_led].level;
	}
	
	if (pin == latch) {
		if (latch_pin.mode == OUTPUT) return latch_pin.level;
		else return HIGH;
	}
	if (pin == clock) {
		if (clock_pin.mode == OUTPUT) return clock_pin.level;
		else return HIGH;
	}
	if (pin == data) {
		if (data_pin.mode == OUTPUT) return data_pin.level;
		else return HIGH;
	}
	return LOW;
}

void Funshield_::digitalWrite(int pin, pin_level value)
{
	if (pinIsLed(pin)) led_pin[pin - first_led].level = value;
	else if (pin == latch) {
		if (latch_pin.mode != OUTPUT) return;
		if (latch_pin.level == LOW && value == HIGH) {
			segm_data = segm_data_buffer;
			segm_pos = segm_pos_buffer;
		}
		latch_pin.level = value;
	}
	// Ignore writes to other pins
}

void Funshield_::shiftOut(int dataPin, int clockPin, bit_order bitOrder, byte value)
{
	if (dataPin == data && clockPin == clock) {
		if (bitOrder == LSBFIRST) {
			// TODO change bit order here
		}
		segm_data_buffer = segm_pos_buffer;
		segm_pos_buffer = value;
	}
}

bool Funshield_::isLedOn(int led)
{
	if (led >= led_count || led < 0) return false;
	return led_pin[led].mode == OUTPUT && led_pin[led].level == LOW;
}

void Funshield_::setButton(int button, bool pressed)
{
	if (button >= button_count || button < 0) return;
	button_pin[button].level = pressed ? LOW : HIGH;
}

byte Funshield_::getGlyph(int pos)
{
	return segm_pos & (1 << pos) ? segm_data : segm_off;
}

unsigned long Funshield_::millis()
{
	return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start_time).count();
}

unsigned long Funshield_::micros()
{
	return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - start_time).count();
}

void delay(unsigned long ms)
{
	std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}
void delayMicroseconds(unsigned int us)
{
	std::this_thread::sleep_for(std::chrono::microseconds(us));
}

long random(long max) 
{ 
	return std::rand() % max; 
}
long random(long min, long max)
{
  if (min >= max) return min;
  return random(max - min) + min;
}

using namespace std;