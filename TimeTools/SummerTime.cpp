#include "SummerTime.h"


uint8_t LastSunday(uint16_t year, uint8_t month) {
  for(int8_t day = 31; day > 0; day--) {
     uint8_t m = (month + 9) % 12;
     uint16_t y = year - m / 10;
     uint32_t dn = 365 * y + y/4 - y/100 + y/400 + (m*306 + 5)/10 + (day - 1);
     dn += 3;
     dn %= 7;
     if (dn == 0)
        return day;    
     }
  return 0; // not found.
}

bool IsEuropeSummerTime(uint16_t y, uint8_t m, uint8_t d, uint8_t H) {
  uint8_t sunday;
  static bool adj_back = false;

  switch(m) {
     case 1: return false;
     case 2: return false;
     case 3: {
        sunday = LastSunday(y, 3);
        if (d < sunday)
           return false;
        if (d > sunday)
           return true;
        /* hmm..
         * this is the day of the normal time to summer time switchover
         * CET: at 02:00, the clock is adjusted to 03:00
         * UTC: at 01:00, the clock is adjusted to 02:00
         */
        return H >= 1;
        }
     case 4: return true;
     case 5: return true;
     case 6: return true;
     case 7: return true;
     case 8: return true;
     case 9: return true;
     case 10: {
        sunday = LastSunday(y, 10);
        if (d < sunday)
           return true;
        if (d > sunday)
           return false;
        /* hmm..
         * this is the day of the summer time to normal time switchover
         * CET: at 03:00, the clock is adjusted to 02:00
         * UTC: at 02:00, the clock is adjusted to 01:00
         */
        switch(H) {
           case 0:
              adj_back = false; // prepare for time back adjustment
              return true;
           case 1:
              if (adj_back) // clock already adusted backwards; just arrived in winter time.
                 return false;
              else          // still in summer, reaching clock adjustment in less than an hour
                 return true;
           case 2:
              adj_back = true;
              return false;
           default:
              return false;
           }
        }
     case 11: return false;
     case 12: return false;
     }
  return false;
}
