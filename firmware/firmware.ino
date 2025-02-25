#include <Wire.h>
#include "NumberDisplay.h"
#include "DS3231.h"

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

void setup(void) {
  Wire.begin();
  Serial.begin(115200);
  while(!Serial)delay(100);
  Serial.println("begin..");
  //tft.setSPISpeed(80000000); // 40 MHz
  tft.init(240, 280, SPI_MODE0, true);
  Serial.println("vor rtc.begin..");
  rtc.begin();
  Serial.println("initialized..");
}

//uint32_t i=0;
uint8_t Seconds { 100 };

void loop(void) {
//  uint32_t n = i++;
  Serial.println(" ..");
  rtc.GetTime(&tm);
//  if (tm.Seconds == Seconds)
//     return;

  char datestr[20];
  rtc.PrintDateTime(&tm, datestr); Serial.println(datestr);



  int8_t colon = -1 + (tm.Seconds & 1) * 11;

  tft.DrawNumber(tm.Seconds % 10, 7); tm.Seconds /= 10;
  tft.DrawNumber(tm.Seconds % 10, 6);

  tft.DrawNumber(colon, 5);

  tft.DrawNumber(tm.Minutes % 10, 4); tm.Minutes /= 10;
  tft.DrawNumber(tm.Minutes % 10, 3);

  tft.DrawNumber(colon, 2);

  tft.DrawNumber(tm.Hours % 10, 1); tm.Hours /= 10;
  tft.DrawNumber(tm.Hours % 10, 0);

  /*struct Time {
     uint8_t Seconds; // 00–59
     uint8_t Minutes; // 00–59
     uint8_t Hours;   // 00–23
     uint8_t Days;    // 1–7
     uint8_t Date;    // 01–31
     uint8_t Month;   // 01-12
     uint16_t Year;   // 20xx
     };

  tft.DrawNumber(n % 10, 7); n /= 10;
  tft.DrawNumber(n % 10, 6); n /= 10;
  tft.DrawNumber(n % 10, 5); n /= 10;
  tft.DrawNumber(n % 10, 4); n /= 10;
*/

  delay(50);

}

