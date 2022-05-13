#include "funshield.h"
using namespace fs;
void setup() {
    // Run once
    pinMode(13, OUTPUT); // Set LED pin as input pin
    pinMode(15, INPUT);  // Set button pin as output pin
}

void loop() {
    // Run repeatedly
	digitalWrite(13, digitalRead(A1)); // Light LED while button is pressed
}