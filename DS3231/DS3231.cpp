#include "DS3231.h"
#include <Wire.h>

/*
 *--------------------------------------------------------------------------------------------------------------
 * ADDRESS  | BIT 7 | BIT 6 | BIT 5 | BIT 4 | BIT 3 | BIT 2 | BIT 1 | BIT 0 | FUNCTION        | RANGE
 *--------------------------------------------------------------------------------------------------------------
 * 00h      | 0     | ---- 10 Seconds ----- | ---------- Seconds ---------- | Seconds         | 00–59
 *          |___________________________________________________________________________________________________
 * 01h      | 0     | ---- 10 Minutes ----- | ---------- Minutes ---------- | Minutes         | 00–59
 *          |___________________________________________________________________________________________________
 * 02h      | 0     |12/n24 |nAM/PM |10 Hour| ---------- Hour ------------- | Hours           | 1–12 + AM/PM
 *          |       |       |20 Hour|       |                               |                 | 00–23
 *          |___________________________________________________________________________________________________
 * 03h      | 0     | 0     | 0     | 0     | 0     | -------- Day -------- | Day             | 1–7
 *          |___________________________________________________________________________________________________
 * 04h      | 0     | 0     | --- 10 Date - | ---------- Date ------------- | Date            | 01–31
 *          |___________________________________________________________________________________________________
 * 05h      |Century| 0     | 0     |10Month| ---------- Month ------------ | Month/Century   | 01–12 + Century
 *          |___________________________________________________________________________________________________
 * 06h      | -------- 10 Year ------------ | ---------- Year ------------- | Year            | 00–99
 *          |___________________________________________________________________________________________________
 * 07h      | A1M1  | ---- 10 Seconds ----- | ---------- Seconds ---------- | Alarm 1 Seconds | 00–59
 *          |___________________________________________________________________________________________________
 * 08h      | A1M2  | ---- 10 Minutes ----- | ---------- Minutes ---------- | Alarm 1 Minutes | 00–59
 *          |___________________________________________________________________________________________________
 * 09h      | A1M3  |12/n24 |nAM/PM  10 Hour| ---------- Hour ------------- | Alarm 1 Hours   | 1–12 + AM/PM
 *          |       |       |20 Hour        |                               |                 | 00–23
 *          |___________________________________________________________________________________________________
 * 0Ah      | A1M4  |DY/nDT |--- 10 Date -- | ---------- Day -------------- | Alarm 1 Day     | 1–7
 *          |       |       |               | ---------- Date ------------- | Alarm 1 Date    | 1–31
 *          |___________________________________________________________________________________________________
 * 0Bh      | A2M2  | ---- 10 Minutes ----- | ---------- Minutes ---------- | Alarm 2 Minutes | 00–59
 *          |___________________________________________________________________________________________________
 * 0Ch      | A2M3  |12/n24 |nAM/PM  10 Hour| ---------- Hour ------------- | Alarm 2 Hours   | 1–12 + AM/PM
 *          |       |       |20 Hour        |                               |                 | 00–23
 *          |___________________________________________________________________________________________________
 * 0Dh      | A2M4  |DY/nDT |--- 10 Date -- | ---------- Day -------------- | Alarm 2 Day     | 1–7
 *          |       |       |               | ---------- Date ------------- | Alarm 2 Date    | 1–31
 *          |___________________________________________________________________________________________________
 * 0Eh      | nEOSC | BBSQW | CONV  | RS2   | RS1   | INTCN | A2IE  | A1IE  | Control         | —
 * 0Fh      | OSF   | 0     | 0     | 0     |EN32kHz| BSY   | A2F   | A1F   | Control/Status  | —
 * 10h      | SIGN  | DATA  | DATA  | DATA  | DATA  | DATA  | DATA  | DATA  | Aging Offset    | —
 * 11h      | SIGN  | DATA  | DATA  | DATA  | DATA  | DATA  | DATA  | DATA  | MSB of Temp     | —
 * 12h      | DATA    DATA    0       0       0       0       0     | 0     | LSB of Temp     | —
 */

static const uint8_t I2C_address = 0x68;

/*******************************************************************************
 * forward decls
 ******************************************************************************/
int BCD2Decimal(uint8_t bcd);
uint8_t DecimalToBCD(int d);
uint8_t TwoCharsToByte(char* s);
uint8_t DayOfWeek(uint16_t y, uint8_t m, uint8_t d);


/*******************************************************************************
 * class DS3231
 ******************************************************************************/

DS3231::DS3231() {
}

void DS3231::begin() {
  Wire.begin();
}

void DS3231::ReadRegister(uint8_t reg, uint8_t* dest) {
  Wire.beginTransmission(I2C_address);
  Wire.write(reg);
  Wire.requestFrom(I2C_address, (uint8_t) 1);
  *dest = Wire.read();
  Wire.endTransmission(true);
}

void DS3231::ReadRegisters(uint8_t reg, uint8_t count, uint8_t* dest) {
  Wire.beginTransmission(I2C_address);
  Wire.write(reg);
  Wire.requestFrom(I2C_address, count);
  for(int i=0; i<count; i++)
     dest[i] = Wire.read();
  Wire.endTransmission(true);
}

void DS3231::WriteRegister(uint8_t reg, uint8_t val) {
  Wire.beginTransmission(I2C_address);
  Wire.write(reg);
  Wire.write(val);
  Wire.endTransmission(true);
}

void DS3231::GetTime(struct Time* tm) {
  ReadRegisters(0, 7, reg);
  tm->Seconds = BCD2Decimal(reg[0x00] & 0x7F);
  tm->Minutes = BCD2Decimal(reg[0x01] & 0x7F);
  bool AM_PM = (reg[0x02] & 0x40) > 0;
  if (AM_PM) {
     tm->Hours = BCD2Decimal(reg[0x02] & 0x1F);
     if ((reg[0x02] & 0x20) > 0)
        tm->Hours += 12;
     }
  else {
     tm->Hours = BCD2Decimal(reg[0x02] & 0x3F);
     }
  tm->Days = reg[0x03] & 0x7;
  tm->Date = BCD2Decimal(reg[0x04] & 0x3F);
  tm->Month = BCD2Decimal(reg[0x05] & 0x1F);
  bool Century = (reg[0x05] & 0x80) > 0;
  tm->Year = BCD2Decimal(reg[0x06]);
  if (Century)
     tm->Year += 2000;
}

void DS3231::SetTime(struct Time* tm) {
  WriteRegister(0x00, DecimalToBCD(tm->Seconds));    // 00–59
  WriteRegister(0x01, DecimalToBCD(tm->Minutes));    // 00–59
  WriteRegister(0x02, DecimalToBCD(tm->Hours));      // 00–23
  WriteRegister(0x03, tm->Days);                     // 1–7
  WriteRegister(0x04, DecimalToBCD(tm->Date));       // 01–31
  uint8_t reg05 = DecimalToBCD(tm->Month);           // 01-12
  if (tm->Year >= 2000) reg05 |= 0x80;               // Century flag
  WriteRegister(0x05, reg05);                        // 01–12 + Century
  WriteRegister(0x06, DecimalToBCD(tm->Year % 100)); // 00–99
}

void DS3231::PrintDateTime(struct Time* tm, char* buf) {
  int y;
  uint8_t p = 0;
  buf[p++] = '0' + tm->Date / 10;
  buf[p++] = '0' + tm->Date % 10;
  buf[p++] = '.';
  buf[p++] = '0' + tm->Month / 10;
  buf[p++] = '0' + tm->Month % 10;
  buf[p++] = '.';
  buf[p++] = '0' + tm->Year / 1000; y = tm->Year % 1000;
  buf[p++] = '0' + y        / 100;  y = y % 100;
  buf[p++] = '0' + y        / 10;   y = y % 10;
  buf[p++] = '0' + y;
  buf[p++] = ' ';
  buf[p++] = '0' + tm->Hours / 10;
  buf[p++] = '0' + tm->Hours % 10;
  buf[p++] = ':';
  buf[p++] = '0' + tm->Minutes / 10;
  buf[p++] = '0' + tm->Minutes % 10;
  buf[p++] = ':';
  buf[p++] = '0' + tm->Seconds / 10;
  buf[p++] = '0' + tm->Seconds % 10;
  buf[p++] =  0;
}

void DS3231::PrintTime(struct Time* tm, char* buf) {
  uint8_t p = 0;
  buf[p++] = '0' + tm->Hours / 10;
  buf[p++] = '0' + tm->Hours % 10;
  buf[p++] = ':';
  buf[p++] = '0' + tm->Minutes / 10;
  buf[p++] = '0' + tm->Minutes % 10;
  buf[p++] = ':';
  buf[p++] = '0' + tm->Seconds / 10;
  buf[p++] = '0' + tm->Seconds % 10;
  buf[p++] =  0;
}

void DS3231::ParseDateTime(struct Time* tm, char* datetime) {
  // check string to be 'dd.mm.yyyy HH:MM:SS'
  for(int i=0; i<19; i++) if (datetime[i] == 0) return;
  if (datetime[19] != 0) return;
  if (datetime[ 2] != '.') return;
  if (datetime[ 5] != '.') return;
  if (datetime[13] != ':') return;
  if (datetime[16] != ':') return;

  tm->Date    = TwoCharsToByte(datetime); datetime += 3;  // 01–31
  tm->Month   = TwoCharsToByte(datetime); datetime += 3;  // 01-12
  tm->Year    = TwoCharsToByte(datetime); datetime += 2;  // 20xx
  tm->Year   *= 100;
  tm->Year   += TwoCharsToByte(datetime); datetime += 3;  // 20xx
  tm->Days    = DayOfWeek(tm->Year, tm->Month, tm->Date); // 1–7
  tm->Hours   = TwoCharsToByte(datetime); datetime += 3;  // 00–23
  tm->Minutes = TwoCharsToByte(datetime); datetime += 3;  // 00–59
  tm->Seconds = TwoCharsToByte(datetime);                 // 00–59
}

int BCD2Decimal(uint8_t bcd) {
  return (bcd & 0xF) + 10 * (bcd >> 4);
}

uint8_t DecimalToBCD(int d) {
  return (d % 10) | ((d / 10) << 4);
}

uint8_t TwoCharsToByte(char* s) {
  uint8_t u  = (*s++ - '0') * 10;
          u += (*s   - '0');
  return u;
}

uint8_t DayOfWeek(uint16_t y, uint8_t m, uint8_t d) {
  m = (m + 9) % 12;
  y -= m / 10;
  uint32_t dn = 365 * y + y/4 - y/100 + y/400 + (m*306 + 5)/10 + (d - 1);
  dn += 3;
  dn %= 7;
  if (dn == 0) dn = 7;
  return dn;
}
