#pragma once

#include <cstdint>

/* This class maintains the information, which digit of our display
 * is currently written to, for up to 254 digits.
 */
class Multiplexer {
private:
  int8_t* pins;
  uint8_t count;
  uint8_t selected;
public:
  Multiplexer(int8_t* Pins, uint8_t Count);
  ~Multiplexer();

  // zero-based, as usual.
  void Select(uint8_t Digit);
  void SelectAll(void);
  void SelectNone(void);
};
