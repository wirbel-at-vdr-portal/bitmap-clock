/*******************************************************************************
 * This file is part of bitmap-clock.
 * https://github.com/wirbel-at-vdr-portal/bitmap-clock
 * See the README file for copyright information and how to reach the author.
 ******************************************************************************/

/*******************************************************************************
 * NumberDisplay is an Adafruit_GFX derived class for
 * displaying numbers on a 1.69" 280x240 ST7789 IPS LCD
 ******************************************************************************/


#include "NumberDisplay.h"
#include "bitmaps/Ziffer_0.h"
#include "bitmaps/Ziffer_1.h"
#include "bitmaps/Ziffer_2.h"
#include "bitmaps/Ziffer_3.h"
#include "bitmaps/Ziffer_4.h"
#include "bitmaps/Ziffer_5.h"
#include "bitmaps/Ziffer_6.h"
#include "bitmaps/Ziffer_7.h"
#include "bitmaps/Ziffer_8.h"
#include "bitmaps/Ziffer_9.h"
#include "bitmaps/DP.h"
#include "bitmaps/DP_off.h"

NumberDisplay::NumberDisplay(int8_t ChipSelects[], uint8_t Digits, int8_t cs, int8_t dc, int8_t rst) :
  Adafruit_ST77xx(240, 280, cs, dc, rst), Multiplexer(ChipSelects, Digits), values({~0}) {}

NumberDisplay::NumberDisplay(int8_t ChipSelects[], uint8_t Digits, int8_t cs, int8_t dc, int8_t mosi, int8_t sclk, int8_t rst) :
  Adafruit_ST77xx(240, 280, cs, dc, mosi, sclk,rst), Multiplexer(ChipSelects, Digits), values({~0}) {}

static const uint8_t PROGMEM generic_st7789[] = { // Init commands for 7789 screens
  9,                              // 9 commands in list:
  ST77XX_SWRESET,   ST_CMD_DELAY, // 1: Software reset, no args, w/delay
  150,                            //    ~150 ms delay
  ST77XX_SLPOUT ,   ST_CMD_DELAY, // 2: Out of sleep mode, no args, w/delay
  10,                             //     10 ms delay
  ST77XX_COLMOD , 1+ST_CMD_DELAY, // 3: Set color mode, 1 arg + delay:
  0x55,                           //    16-bit color
  10,                             //    10 ms delay
  ST77XX_MADCTL , 1,              // 4: Mem access ctrl (directions), 1 arg:
  0x08,                           //    Row/col addr, bottom-top refresh
  ST77XX_CASET  , 4,              // 5: Column addr set, 4 args, no delay:
  0x00,                           //
  0,                              //    XSTART = 0
  0,                              //
  240,                            //    XEND = 240
  ST77XX_RASET  , 4,              // 6: Row addr set, 4 args, no delay:
  0x00,                           //
  0,                              //    YSTART = 0
  320 >>8,                //
  320 &0xFF,              //    YEND = 320
  ST77XX_INVON  ,   ST_CMD_DELAY, // 7: hack
  10,                             //
  ST77XX_NORON  ,   ST_CMD_DELAY, // 8: Normal display on, no args, w/delay
  10,                             //    10 ms delay
  ST77XX_DISPON ,   ST_CMD_DELAY, // 9: Main screen turn on, no args, delay
  10                              //   10 ms delay
};

void NumberDisplay::init(uint16_t width, uint16_t height, uint8_t mode, bool spi_init) {
  SelectAll();
  spiMode = mode;

  // Adafruit_ST77xx::commonInit() -> Adafruit_ST77xx::begin() -> Adafruit_SPITFT::initSPI()
  // This may cause trouble for multiple SPI TFTs, so init SPI only if requested..
  // commonInit(NULL);

  #define SPI_DEFAULT_FREQ 32000000 // Default SPI data clock frequency

  uint32_t freq = SPI_DEFAULT_FREQ;
  invertOnCommand  = ST77XX_INVON;
  invertOffCommand = ST77XX_INVOFF;
  if (spi_init)
     initSPI(freq, spiMode);


  if ((width == 240) && (height == 240)) { // 1.3" and 1.54" displays
     _colstart = 0;
     _rowstart = 80;
     }
  else if ((width == 135) && (height == 240)) { //1.13" display
     _colstart = 53;
     _rowstart = 40;
     }
  else {
     _colstart = 0;
     _rowstart = 20;
     }
  WIDTH = width;
  HEIGHT = height;
  _width    = width;
  _height   = height;

  displayInit(generic_st7789);
  setRotation(0);
  invertDisplay(true);
  DrawNumber(0);
  SelectNone();
}

void NumberDisplay::setRotation(uint8_t m) {
  uint8_t madctl;

  rotation = m & 3;
  switch(rotation) {
     case 0:
        madctl  = ST77XX_MADCTL_MX | ST77XX_MADCTL_MY | ST77XX_MADCTL_RGB;
        _xstart = _colstart;
        _ystart = _rowstart;
        _width = WIDTH;
        _height = HEIGHT;
        break;
     case 1:
        madctl  = ST77XX_MADCTL_MY | ST77XX_MADCTL_MV | ST77XX_MADCTL_RGB;
        _xstart = _rowstart;
        _ystart = _colstart;
        _height = WIDTH;
        _width = HEIGHT;
        break;
     case 2:
        madctl  = ST77XX_MADCTL_RGB;
        if ((WIDTH == 135) && (HEIGHT == 240)) { 
           _xstart = _colstart-1;
           _ystart = _rowstart;
           }
        else {
           _xstart = 0;
           _ystart = 0;
           }
        _width = WIDTH;
        _height = HEIGHT;
        break;
     case 3:
        madctl  = ST77XX_MADCTL_MX | ST77XX_MADCTL_MV | ST77XX_MADCTL_RGB;
        if ((WIDTH == 135) && (HEIGHT == 240)) { 
           _xstart = _rowstart;
           _ystart = _colstart;
           }
        else {
           _xstart = 0;
           _ystart = 0;
           }
        _height = WIDTH;
        _width = HEIGHT;
        break;
     }

  sendCommand(ST77XX_MADCTL, &madctl, 1);
}

void NumberDisplay::DrawNumber(int n, uint8_t digit) {
  if (values[digit] == n)
     return;

  values[digit] = n;
  Select(digit);
  DrawNumber(n);
}

void NumberDisplay::DrawNumber(int n) {
  switch(n) {
     case -1:
        drawRGBBitmap(0,0, (uint16_t*) DP, 240,280, true, false);
        break;
     case 0:
        drawRGBBitmap(0,0, (uint16_t*) Ziffer_0, 240,280, true, false);
        break;
     case 1:
        drawRGBBitmap(0,0, (uint16_t*) Ziffer_1, 240,280, true, false);
        break;
     case 2:
        drawRGBBitmap(0,0, (uint16_t*) Ziffer_2, 240,280, true, false);
        break;
     case 3:
        drawRGBBitmap(0,0, (uint16_t*) Ziffer_3, 240,280, true, false);
        break;
     case 4:
        drawRGBBitmap(0,0, (uint16_t*) Ziffer_4, 240,280, true, false);
        break;
     case 5:
        drawRGBBitmap(0,0, (uint16_t*) Ziffer_5, 240,280, true, false);
        break;
     case 6:
        drawRGBBitmap(0,0, (uint16_t*) Ziffer_6, 240,280, true, false);
        break;
     case 7:
        drawRGBBitmap(0,0, (uint16_t*) Ziffer_7, 240,280, true, false);
        break;
     case 8:
        drawRGBBitmap(0,0, (uint16_t*) Ziffer_8, 240,280, true, false);
        break;
     case 9:
        drawRGBBitmap(0,0, (uint16_t*) Ziffer_9, 240,280, true, false);
        break;
     default:
        drawRGBBitmap(0,0, (uint16_t*) DP_off, 240,280, true, false);
     }
}

void NumberDisplay::drawRGBBitmap(int16_t x, int16_t y,
                                  uint16_t *pcolors,
                                  int16_t w, int16_t h,
                                  bool block,
                                  bool bigEndian) {

  int16_t x2, y2;

  if ((x >= _width) ||
      (y >= _height) ||
      ((x2 = (x + w - 1)) < 0) ||
      ((y2 = (y + h - 1)) < 0))
     return;


  int16_t bx1 = 0, by1 = 0, saveW = w;

  if (x < 0) {
     w += x;
     bx1 = -x;
     x = 0;
     }

  if (y < 0) {
     h += y;
     by1 = -y;
     y = 0;
     }

  if (x2 >= _width)
     w = _width - x;

  if (y2 >= _height)
     h = _height - y;

  pcolors += by1 * saveW + bx1; // Offset bitmap ptr to clipped top-left
  startWrite();
  setAddrWindow(x, y, w, h);
  while (h--) {                                 // For each (clipped) scanline...
     writePixels(pcolors, w, block, bigEndian); // Push one (clipped) row
     pcolors += saveW;                          // Advance pointer by one full (unclipped) line
     }
  endWrite();
}
