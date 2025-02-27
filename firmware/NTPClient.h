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

/* 02/2025 -wk: This file was edited and reduced to bare minimum */

#pragma once
#include <Arduino.h>
#include <Udp.h>

class NTPClient {
  private:
    UDP* udp;
    bool initialized;
    const char* ServerName;
    uint32_t EpochTime; // time_t, secs since 01.01.1970
    unsigned long lastMsecs;
    uint8_t     buf[48];
  public:
    NTPClient(UDP& UDP_Class);

    /**
     * Starts the underlying UDP client with the specified local port
     */
    void begin(uint16_t port = 1337);

    /**
     * Stops the underlying UDP client
     */
    void end();

    /**
     * This should be called in the main loop of your application. By default an update from the NTP Server is only
     * made every 60 seconds. This can be configured in the NTPClient constructor.
     * @return true on success, false on failure
     */
    bool update();

    /**
     * Set time server name
     * @param Name: the server name
     */
    void setServerName(const char* Name);

    /**
     * @returns seconds since Jan. 1, 1970
     */
    uint32_t getEpochTime() const;
};
