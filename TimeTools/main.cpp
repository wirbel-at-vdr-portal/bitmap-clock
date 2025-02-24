#include "SummerTime.h"
#include <iostream>

int main() {
  for(int year=2024; year<=2030; year++) {
     uint8_t summer_start = LastSunday(year,  3);
     uint8_t summer_stop  = LastSunday(year, 10);
     
     for(int utc_hr = 0; utc_hr < 24; utc_hr++) {
        std::cout << (int) summer_start << ".03." << year
                  << " " << ((utc_hr<10)?"0":"") << utc_hr << ":00 = " << (IsEuropeSummerTime(year, 3,summer_start, utc_hr)?"true":"false") << "  -  ";
        std::cout << (int) summer_start << ".10." << year
                  << " " << ((utc_hr<10)?"0":"") << utc_hr << ":00 = " << (IsEuropeSummerTime(year,10,summer_stop , utc_hr)?"true":"false") << std::endl;
        }
     }

  return 0;
}
