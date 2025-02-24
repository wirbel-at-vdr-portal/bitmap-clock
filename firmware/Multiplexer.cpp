#include "Multiplexer.h"
#include <Arduino.h>


Multiplexer::Multiplexer(int8_t* Pins, uint8_t Count) :
  pins(Pins), count(Count), selected(~0) {
  for(int i=0; i<count; i++) {
     pinMode(pins[i], OUTPUT);
     digitalWrite(pins[i], HIGH);
     }
}

Multiplexer::~Multiplexer() {
  for(int i=0; i<count; i++) {
     digitalWrite(pins[i], HIGH);
     pinMode(pins[i], INPUT);
     }
}

void Multiplexer::Select(uint8_t Digit) {
  if (selected != ~0)
     digitalWrite(pins[selected], HIGH);
  selected = Digit;
  digitalWrite(pins[selected], LOW);
}

void Multiplexer::SelectAll(void) {
  selected = ~0;
  for(int i=0; i<count; i++) {
     digitalWrite(pins[i], LOW);
     }
}

void Multiplexer::SelectNone(void) {
  selected = ~0;
  for(int i=0; i<count; i++) {
     digitalWrite(pins[i], HIGH);
     }
}
