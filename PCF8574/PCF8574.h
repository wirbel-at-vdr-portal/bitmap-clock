/*******************************************************************************
 * This file is part of bitmap-clock.
 * https://github.com/wirbel-at-vdr-portal/bitmap-clock
 * See the README file for copyright information and how to reach the author.
 ******************************************************************************/

#pragma once
#include <stdint.h> // uint8_t

class PCF8574 {
private:
  uint8_t I2C_address;
  uint8_t state;
public:
  /* PCF8574 : Address = 0x20 + 2*A[2:0]
   * PCF8574A: Address = 0x38 + 2*A[2:0]
   */
  PCF8574(uint8_t Address);

  void begin(void);

  /* Read all bits into dest
   * val[7:0] <-> P[0:7]
   * returns true on success
   */
  bool Read(uint8_t* dest);

  /* Set all bits.
   * val[7:0] <-> P[0:7]
   * returns true on success
   */
  bool Write(uint8_t val);

  /* Bit = 0..7
   * return 0 or 1;
   */
  uint8_t ReadBit(uint8_t Bit);

  /* Bit = 0..7
   * Value = { 0, 1 }
   * returns true on success
   */
  bool WriteBit(uint8_t Bit, uint8_t Value);
};
