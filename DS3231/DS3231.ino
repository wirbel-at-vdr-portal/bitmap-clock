#include "DS3231.h"

auto rtc = DS3231();
DS3231::Time tm;
uint8_t buf[20];

void setup() {
  Serial.begin(115200);
  rtc.begin();
  //rtc.Enable32kHzOutput(false);
  rtc.ReadRegisters(0, 1+0x12, buf);
  Serial.print("Enable Oscillator on Battery:      "), Serial.println((buf[0xE] & 0x80) > 0?"false":"true");
  Serial.print("Battery-Backed Square-Wave Enable: "), Serial.println((buf[0xE] & 0x40) > 0?"true":"false");
  const char* rates[] = {"1Hz","1.024kHz","4.096kHz", "8.192kHz"};
  Serial.print("Rate Select (RS2 and RS1):         "), Serial.println(rates[(buf[0xE] & 0x1F) >> 3]);
  const char* INTCN[] = {"square wave output", "alarm output"};
  Serial.print("Interrupt Control:                 "), Serial.println(INTCN[(buf[0xE] & 0x04) > 0]);
  Serial.print("Alarm 2 Interrupt Enable:          "), Serial.println((buf[0xE] & 0x02) > 0);
  Serial.print("Alarm 1 Interrupt Enable:          "), Serial.println((buf[0xE] & 0x01) > 0);
  Serial.print("Enable 32kHz Output:               "), Serial.println((buf[0xF] & 0x08) > 0);
  Serial.print("Aging Offset:                      "), Serial.println((int) buf[0x10]);
}

void loop() {

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

  rtc.ReadRegisters(0, 1+0x12, buf);

  for(int16_t i=0; i<=0x12; i++) {
     Serial.print(buf[i] < 16 ? "0" : "");
     Serial.print(buf[i], HEX);
     Serial.print(", ");
     }
  Serial.println("");

  rtc.GetTime(&tm);
  char datestr[20];
  rtc.PrintDateTime(&tm, datestr); Serial.println(datestr);

  Serial.print("T = "), Serial.print(rtc.Temperature()), Serial.println("°C");

  delay(1000);
}
