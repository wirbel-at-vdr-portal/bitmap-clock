
#include "NumberDisplay.h"


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


void setup(void) {
  tft.setSPISpeed(80000000); // 40 MHz
  tft.init(240, 280, SPI_MODE0, true);
}

uint32_t i=0;

void loop(void) {
  //Digits.SelectAll();
  uint32_t n = i++;

  tft.DrawNumber(n % 10, 7); n /= 10;
  tft.DrawNumber(n % 10, 6); n /= 10;
  tft.DrawNumber(n % 10, 5); n /= 10;
  tft.DrawNumber(n % 10, 4); n /= 10;
  tft.DrawNumber(n % 10, 3); n /= 10;
  tft.DrawNumber(n % 10, 2); n /= 10;
  tft.DrawNumber(n % 10, 1); n /= 10;
  tft.DrawNumber(n % 10, 0); n /= 10;
  delay(50);

}

