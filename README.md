The idea of an 6-digit (actually eight: six numbers plus two digits showing a blinking separator) Nixie-like clock, using eight 1.69" 280x240 ST7789 IPS LCDs.

It should display the time as hh:mm:ss (eight displays)

Not yet shure if that idea may work at all..
I'm just collecting pieces yet.

Components, which may change:
* Arduino Nano ESP32 (Nora), it features a 16 MByte external flash via QSPI, running at up to 83 Mbyte/s.
* RTC DS3231SN
* 8x 1.69" 280x240 ST7789 IPS LCD
* 4x 74LVT244A + 8x 1k pullup resistors for driving eight chip select pins
* Implementing or Re-Implementing lib NTPClient? Depends on avail power/space.
* (what else..?)

![Ziffer 9](doc/Ziffer.jpg)

![overview](doc/overview_small.bmp)

My Nano ESP32 might be a bit tight in terms of program space for such a number of images.
But, I would like to avoid additional memory - more pins, more effort.
[edit]20250223: No, i don't need more memory, driving all digits takes less than 60% of the avail memory.[/edit]

Therefore, the IPS displays and the RTC should use tiny(!) libs.

It turned out, that the A(..)t ST7779 lib is tooo slooww (..zzZZZzz..) to render a bitmap in less than a quarter of a second.
Not possible to display seconds that way, but meanwhile solved.

Putting eight SPI displays on a bus needs testing in any case, the controllers output may struggle to drive them. Or, i need to slow down SPI, which will hurt again.
[edit]20250223: No problem at all, the deafult of 32MHz runs nice. Setting it SPI clk to 80MHz doesn't change anything - to be measured.[/edit]

Measured current consumption:
* 5V @ 280mA
* This is for running eight displays at max speed, with the RTC idle.

Display update Time:
* to update all eight digits, we need ~ 0.5sec
* i can save time, if updating only digits, which actually changed their value.



== Current Status ==
* 20250223
** prototype pcb assembled.
** prototype board needs changes, eight pullup resistors are missing. For first tests, i added those to the display pcbs.
** otherwise, display speed is sufficient
** SPI runs stable at least at 32MHz
** 4 spare pcb's plus parts to play with.
** first successful firmware.
** IPS displays are actually GREAT in terms of display angle. 
