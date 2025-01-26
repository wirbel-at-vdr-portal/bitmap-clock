The idea of an 6-digit Nixie-like clock, using eight 1.69" 280x240 ST7789 IPS LCDs.

It should display the time as hh:mm:ss (eight displays)

Not yet shure if i that idea may work at all..

Components, which may change:
* Arduino Nano ESP32 (Nora)
* RTC DS3231SN
* 8x 1.69" 280x240 ST7789 IPS LCD
* (what else..?)

![Ziffer 9](doc/Ziffer.jpg)

My Nano ESP32 might be a bit tight in terms of program space for such a number of images.
But, I would like to avoid additional memory - more pins, more effort.

Therefore, the IPS displays and the RTC should use tiny(!) libs.

It turned out, that the A(..)t ST7779 lib is tooo slooww (..zzZZZzz..) to render a bitmap in less than a quarter of a second.
Not possible to display seconds that way, but meanwhile solved.

Putting eight SPI displays on a bus needs testing in an<y case, the controllers output may struggle to drive them. Or, i need to slow down SPI, which will hurt again.
