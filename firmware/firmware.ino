/*******************************************************************************
 * This file is part of bitmap-clock.
 * https://github.com/wirbel-at-vdr-portal/bitmap-clock
 * See the README file for copyright information and how to reach the author.
 ******************************************************************************/

#include <Wire.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <TimeLib.h> // https://github.com/PaulStoffregen/Time
#include "NumberDisplay.h"
#include "DS3231.h"
#include "NTPClient.h"
#include "SummerTime.h"
#include "credentials.h"

/*******************************************************************************
 * forward decls
 ******************************************************************************/
void StartWifi();
void ReconnectWifi();


/***************************************************************
 *      TFT        |  Nano
 * ----------------|--------------------------------------------
 *      GND        |  GND
 *      VCC        |  3V3
 *      SCL        |  13 / SCK
 *      SDA        |  11 / COPI (MOSI) */  
#define TFT_CS        10 // Chip Select
#define TFT_RST       9  // LCD reset. -1 for Arduino RESET pin
#define TFT_DC        8
/*      BLK        |  NC // default on, low for shutdown
 ***************************************************************/
int8_t  CS_pins[] = { A6, A7, D7, D6, D5, D4, D3, D2 }; // left to right.

NumberDisplay tft = NumberDisplay(CS_pins, sizeof(CS_pins)/sizeof(CS_pins[0]),
                                  TFT_CS, TFT_DC, TFT_RST);

auto rtc = DS3231();
DS3231::Time tm;
WiFiUDP udp;
NTPClient Ntp(udp);
uint8_t Seconds = 100;
uint32_t updateCount = 0;
int timeZone = +1; // CET or UTC+1


void setup(void) {
  Wire.begin();
  Serial.begin(115200);
  tft.init(240, 280, SPI_MODE0, true);
  rtc.begin();
  StartWifi();
}


void loop(void) {
  rtc.GetTime(&tm);
  if (tm.Seconds == Seconds) {
     delay(10);
     return;
     }
  Seconds = tm.Seconds;

  if (++updateCount >= 15 * 60) {
     updateCount = 0;
     Serial.println("Getting NTP time..");

     if (Ntp.update()) {
        Serial.println("NTP success.");

        time_t t = Ntp.getEpochTime();
        t += timeZone * 3600;

        int hours   = hour(t);
        int days    = day(t);
        int months  = month(t);
        int years   = year(t);
        int minutes;
        int seconds;

        if (IsEuropeSummerTime(years, months, days, hours)) {
           // time is moved one hour forward in local time
           t += 3600;
           hours   = hour(t);
           days    = day(t);
           months  = month(t);
           years   = year(t);
           }

        minutes = minute(t);
        seconds = second(t);

        rtc.GetTime(&tm);
        if (tm.Seconds != seconds or // 00–59
            tm.Minutes != minutes or // 00–59
            tm.Hours   != hours   or // 00–23
            tm.Date    != days    or // 01–31
            tm.Month   != months  or // 01-12
            tm.Year    != years) {   // 20xx
           time_t t2 = rtc.Time_t(&tm) - t;
           tm.Seconds = seconds;
           tm.Minutes = minutes;
           tm.Hours   = hours;
           tm.Date    = days;
           tm.Month   = months;
           tm.Year    = years;
           rtc.SetTime(&tm);
           Serial.print("corrected local RTC by ");
           Serial.print((int) t2);
           Serial.println(" seconds.");
           }
        }
     else if (WiFi.status() != WL_CONNECTED) {
        Serial.print("reconnecting WiFi.. ");
        ReconnectWifi();
        Serial.println(WiFi.status() == WL_CONNECTED? "OK":"nOK");
        }
     }

  int8_t colon = -1 + (tm.Seconds & 1) * 11;
  tft.DrawNumber(colon, 5);
  tft.DrawNumber(colon, 2);

  tft.DrawNumber(tm.Seconds % 10, 7); tm.Seconds /= 10;
  tft.DrawNumber(tm.Seconds % 10, 6);
  tft.DrawNumber(tm.Minutes % 10, 4); tm.Minutes /= 10;
  tft.DrawNumber(tm.Minutes % 10, 3);
  tft.DrawNumber(tm.Hours   % 10, 1); tm.Hours /= 10;
  tft.DrawNumber(tm.Hours   % 10, 0);
}


void StartWifi() {
  int status = WiFi.begin(ssid, password);
  // wait up to 15 secs for connect.
  for(int i=0; (status != WL_CONNECTED) and (i<3); i++)
     status = WiFi.begin(ssid, password);
  Ntp.begin();
}

void ReconnectWifi() {
  Ntp.end();
  StartWifi();
}
