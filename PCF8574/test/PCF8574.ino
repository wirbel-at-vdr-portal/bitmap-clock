/*******************************************************************************
 * This file is part of bitmap-clock.
 * https://github.com/wirbel-at-vdr-portal/bitmap-clock
 * See the README file for copyright information and how to reach the author.
 ******************************************************************************/

#include "PCF8574.h"

auto port_extender = PCF8574(0x20);

void setup() {
  Serial.begin(115200);
  port_extender.begin();
}

void loop() {
  //  port_extender.Write(0xFF);
  //  port_extender.Write(0x00);

  for(int i=0; i<8; i++) {
     port_extender.WriteBit(i, 1);
     delay(25);
     }

  for(int i=0; i<8; i++) {
     port_extender.WriteBit(i, 0);
     delay(25);
     }

}

