#include "funshield.h"
using namespace fs;
void setup() {
    // Run once
    pinMode(13, OUTPUT); // Nastav�me re�im pinu, kde je dioda p�ipojen�, na v�stupn�
}

void loop() {
    // Run repeatedly
	digitalWrite(13, digitalRead(13) ? LOW : HIGH); // Zaps�n�m LOW na pin, kde je dioda p�ipojen�, LED rozsv�t�me
}