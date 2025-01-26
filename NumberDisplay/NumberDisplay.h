/* NumberDisplay is an Adafruit_GFX derived class for
 * displaying numbers on a 1.69" 280x240 ST7789 IPS LCD
 */

#include <Adafruit_ST77xx.h>

class NumberDisplay : public Adafruit_ST77xx {
public:
  NumberDisplay(int8_t cs, int8_t dc, int8_t rst);
  NumberDisplay(int8_t cs, int8_t dc, int8_t mosi, int8_t sclk, int8_t rst = -1);
  NumberDisplay(SPIClass *spiClass, int8_t cs, int8_t dc, int8_t rst);

  void setRotation(uint8_t m);
  void init(uint16_t width, uint16_t height, uint8_t spiMode = SPI_MODE0, bool spi_init = true);

  /*
  Draw a '565' RGB array of pixels to the display.
  Uses  startWrite() and setAddrWindow() calls.

  block
     if true (default case if unspecified), function blocks until DMA transfer
     is complete. This is simply IGNORED, if DMA is not enabled. If false,
     the function returns immediately after the last DMA transfer is started,
     and one should use the dmaWait() function before doing ANY other display-
     related activities (or even any SPI-related activities, if using an SPI
     display that shares the bus with other devices).
  bigEndian
     if true, bitmap in memory is in big-endian order (MSB first). By default
     this is false, as most microcontrollers seem to be little-endian and
     16-bit pixel values must be byte-swapped before issuing to the display
     (which tend toward big-endian when using SPI or 8-bit parallel). If an
     application can optimize around this -- for example, a bitmap in a
     uint16_t array having the byte values already ordered big-endian, this
     can save time here, ESPECIALLY if using this function's non-blocking DMA 
     mode.
   */
  void drawRGBBitmap(int16_t x, int16_t y,
                     uint16_t *pcolors,
                     int16_t w, int16_t h,
                     bool block = true,
                     bool bigEndian = false
                     );

  /* n = 0..9 draws n
   * n = -1 draws a colon.
   * other number clear the screen. 
   */
  void DrawNumber(int n);
};
