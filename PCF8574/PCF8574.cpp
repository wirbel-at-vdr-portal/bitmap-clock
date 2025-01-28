/*******************************************************************************
 * This file is part of bitmap-clock.
 * https://github.com/wirbel-at-vdr-portal/bitmap-clock
 * See the README file for copyright information and how to reach the author.
 ******************************************************************************/

#include "PCF8574.h"
#include <Wire.h>

PCF8574::PCF8574(uint8_t Address) :
 I2C_address(Address) {}

void PCF8574::begin() {
  Wire.begin();
  if (not Write(0))
     Serial.print("No PCF8574 found at 0x"),
     Serial.println(I2C_address, HEX);
}

bool PCF8574::Read(uint8_t* dest) {
  Wire.beginTransmission(I2C_address);
  *dest = Wire.read();
  return Wire.endTransmission() == 0;
}

bool PCF8574::Write(uint8_t val) {
  Wire.beginTransmission(I2C_address);
  Wire.write((state = val));
  return Wire.endTransmission() == 0;
}

uint8_t PCF8574::ReadBit(uint8_t Bit) {
  uint8_t dest;
  Read(&dest);
  return dest & (1 << Bit);
}

bool PCF8574::WriteBit(uint8_t Bit, uint8_t Value) {
  if (Value)
     state |= (1 << Bit);
  else
     state &= ~(1 << Bit);
  return Write(state);
}

