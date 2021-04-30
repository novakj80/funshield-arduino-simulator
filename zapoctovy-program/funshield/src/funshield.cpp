#include "funshield.h"

#include <thread>
fs::Funshield_ fs::Funshield_::instance{};
fs::Funshield_& fs::Funshield_::getInstance()
{
	return fs::Funshield_::instance;
}
fs::serial_monitor fs::Serial;
void fs::pinMode(int pin, fs::pin_mode mode)
{
	fs::Funshield_::getInstance().pinMode(pin, mode);
}

void fs::digitalWrite(int pin, fs::pin_level value) 
{
	fs::Funshield_::getInstance().digitalWrite(pin, value);
}
fs::pin_level fs::digitalRead(int pin) 
{ 
	return fs::Funshield_::getInstance().digitalRead(pin);
}
void fs::shiftOut(int dataPin, int clockPin, fs::bit_order bitOrder, fs::byte value) { fs::Funshield_::getInstance().shiftOut(dataPin, clockPin, bitOrder, value); }
unsigned long fs::millis() { return fs::Funshield_::getInstance().millis(); }
unsigned long fs::micros() { return fs::Funshield_::getInstance().micros(); }
void fs::Funshield_::pinMode(int pin, fs::pin_mode mode)
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

fs::pin_level fs::Funshield_::digitalRead(int pin)
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

void fs::Funshield_::digitalWrite(int pin, fs::pin_level value)
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

void fs::Funshield_::shiftOut(int dataPin, int clockPin,  bit_order bitOrder, byte value)
{
	if (dataPin == data && clockPin == clock) {
		if (bitOrder == LSBFIRST) {
			value = (value & 0xF0) >> 4 | (value & 0x0F) << 4;
			value = (value & 0xCC) >> 2 | (value & 0x33) << 2;
			value = (value & 0xAA) >> 1 | (value & 0x55) << 1;			
		}
		segm_data_buffer = segm_pos_buffer;
		segm_pos_buffer = value;
	}
}

bool fs::Funshield_::isLedOn(int led)
{
	if (led >= led_count || led < 0) return false;
	return led_pin[led].mode == OUTPUT && led_pin[led].level == LOW;
}

void fs::Funshield_::setButton(int button, bool pressed)
{
	if (button >= button_count || button < 0) return;
	button_pin[button].level = pressed ? LOW : HIGH;
}

fs::byte fs::Funshield_::getGlyph(int pos)
{
	return segm_pos & (1 << pos) ? segm_data : segm_off;
}

fs::byte fs::Funshield_::getPositionBitmask()
{
	return segm_pos;
}

unsigned long fs::Funshield_::millis()
{
	return (unsigned long)std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start_time).count();
}

unsigned long fs::Funshield_::micros()
{
	return (unsigned long)std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - start_time).count();
}

void fs::delay(unsigned long ms)
{
	std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}
void fs::delayMicroseconds(unsigned int us)
{
	std::this_thread::sleep_for(std::chrono::microseconds(us));
}

long fs::random(long max) 
{ 
	return std::rand() % max; 
}

long fs::random(long min, long max)
{
  if (min >= max) return min;
  return fs::random(max - min) + min;
}