#include "FunShield.h"
Funshield_ fs_;
void pinMode(int pin, pin_mode mode)
{
	fs_.pinMode(pin, mode);
}

void Funshield_::pinMode(int pin, pin_mode mode)
{
	if (pin >= first_led && pin < first_led + led_count) {
		// set pin mode of led
		int i = pin - first_led;
		led_pin[i].mode = mode;
		led_pin[i].level = mode == OUTPUT ? LOW : HIGH;
	}
	else if (pin >= first_button && pin < first_button + button_count) {
		button_pin[pin - first_button].mode = mode;
	}
	else switch (pin) {
	case latch:
		latch_pin.mode = mode; break;
	case clock:
		clock_pin.mode = mode; break;
	case data:
		data_pin.mode = mode; break;
	}
}

using namespace std;

int main()
{
	cout << "Hello CMake." << endl;
	return 0;
}
