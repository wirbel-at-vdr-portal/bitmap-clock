/**
 * The MIT License (MIT)
 * Copyright (c) 2015 by Fabrice Weinberg
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#include "NTPClient.h"

NTPClient::NTPClient(UDP& UDP_Class) : initialized(false),
   ServerName("pool.ntp.org"), EpochTime(0), lastMsecs(0)
{
  udp = &UDP_Class;
}

void NTPClient::begin(uint16_t Port) {
  udp->begin(Port);
  initialized = true;
}

void NTPClient::end() {
  udp->stop();
  initialized = false;
}

bool NTPClient::update() {
  if (!initialized)
     return false;

  // flush any existing packets
  while(udp->parsePacket())
     udp->flush();

  // Initialize values needed to form NTP request
  memset(&buf[4], 0, sizeof(buf) - 4);
  buf[0]  = 0b11100011; // Leap Indicator: 3 (unknown), NTP Version: 4, Mode: client
  buf[1]  = 0;          // Stratum, or type of clock
  buf[2]  = 6;          // Polling Interval
  buf[3]  = 0xEC;       // Peer Clock Precision
  buf[12] = 49;
  buf[13] = 0x4E;
  buf[14] = 49;
  buf[15] = 52;

  udp->beginPacket(ServerName, 123);
  udp->write(buf, sizeof(buf));
  udp->endPacket();

  unsigned long tm = millis();
  for(int size = 0, timeout = 0; size == 0; timeout++) {
     if (timeout > 100)
        return false;
     size = udp->parsePacket();
     delay(10);
     }

  lastMsecs = tm;
  udp->read(buf, sizeof(buf));

  uint32_t secsSince1900 = ((uint32_t)buf[40]) << 24 |
                           ((uint32_t)buf[41]) << 16 |
                           ((uint32_t)buf[42]) <<  8 |
                                      buf[43];

  // 2208988800: see RFC 868, number of seconds between NTP date 1900-01-01
  //             and unix epoch time 1970-01-01
  EpochTime = secsSince1900 - 2208988800UL;
  return true;  // return true after successful update
}

void NTPClient::setServerName(const char* Name) {
  ServerName = Name;
}

uint32_t NTPClient::getEpochTime() const {
  return EpochTime + (millis() - lastMsecs) / 1000UL;
}
