#include "funshield.h"
using namespace fs;
void setup() {
    // Run once
    pinMode(13, OUTPUT); // Nastavíme režim pinu, kde je dioda pøipojená, na výstupní
}

void loop() {
    // Run repeatedly
	digitalWrite(13, digitalRead(13) ? LOW : HIGH); // Zapsáním LOW na pin, kde je dioda pøipojená, LED rozsvítíme
}