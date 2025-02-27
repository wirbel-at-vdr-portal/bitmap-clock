/*******************************************************************************
 * This file is part of bitmap-clock.
 * https://github.com/wirbel-at-vdr-portal/bitmap-clock
 * See the README file for copyright information and how to reach the author.
 ******************************************************************************/
#pragma once
#include <cstdint>

/* returns the last sunday in a month for a given year and month
 * tested for march and october until 2050.
 * year : UTC, 20xx
 * month: 1..12
 */
uint8_t LastSunday(uint16_t year, uint8_t month);

/* returns for a time in UTC a statement, if there is summer or
 * winter time.
 * y : year as UTC, 20xx
 * m : month as UTC, 1..12
 * d : day as UTC, 1..31
 * H : hour as UTC, 0..23
 */
bool IsEuropeSummerTime(uint16_t y, uint8_t m, uint8_t d, uint8_t H);

