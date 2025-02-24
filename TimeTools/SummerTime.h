#pragma once
#include <cstdint>

/* returns the last sunday in a month for a given year and month
 * tested for march and october until 2050.
 */
uint8_t LastSunday(uint16_t year, uint8_t month);

/* returns for a time in UTC a statement, if there is summer or
 * winter time.
 */
bool IsEuropeSummerTime(uint16_t y, uint8_t m, uint8_t d, uint8_t H);

