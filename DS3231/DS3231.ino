#include "DS3231.h"

auto rtc = DS3231();
DS3231::Time tm;

void setup() {
  Serial.begin(115200);
  if (Serial.available()) Serial.readString();
  rtc.begin();
}

void loop() {
  uint8_t buf[20];

  // set date to 'dd.mm.yyyy HH:MM:SS'
  if (Serial.available() >= 20) {
     char c[22];
     String newdate = Serial.readString();
     newdate.trim();
     Serial.println(newdate);
     newdate.toCharArray(c, sizeof(c));
     rtc.ParseDateTime(&tm, c);
     rtc.SetTime(&tm);
     }
  else if (Serial.available()) {
     Serial.println(Serial.available());
     Serial.readString();
     }

  rtc.ReadRegisters(0, 18, buf);

  for(int i=0; i<18; i++) {
     Serial.print(buf[i] < 16 ? "0" : "");
     Serial.print(buf[i], HEX);
     Serial.print(", ");
     }
  Serial.println("");

  rtc.GetTime(&tm);
  char datestr[20];
  rtc.PrintDateTime(&tm, datestr); Serial.println(datestr);

  delay(1000);
}
