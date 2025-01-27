#pragma once
#include <stdint.h> // uint8_t

class DS3231 {
private:
  uint8_t reg[18];
public:
  struct Time {
     uint8_t Seconds; // 00–59
     uint8_t Minutes; // 00–59
     uint8_t Hours;   // 00–23
     uint8_t Days;    // 1–7
     uint8_t Date;    // 01–31
     uint8_t Month;   // 01-12
     uint16_t Year;   // 20xx
     };
public:
  DS3231();
  void begin(void);
  void ReadRegister(uint8_t reg, uint8_t* dest);
  void ReadRegisters(uint8_t reg, uint8_t count, uint8_t* dest);
  void WriteRegister(uint8_t reg, uint8_t val);
  void GetTime(struct Time* tm);
  void SetTime(struct Time* tm);

  /* When true, the oscillator is started. When set to false,
   * the oscillator is stopped when the DS3231 switches to VBAT
   */
  void EnableOscillator(bool On);

  /* Prints date and time into a char buffer, ie '26.01.2025 10:56:04'
   * buf - pointer to an char[20] (at least)
   */
  static void PrintDateTime(struct Time* tm, char* buf);

  /* Prints time into a char buffer, ie '10:56:04'
   * buf - pointer to an char[9] (at least)
   */
  static void PrintTime(struct Time* tm, char* buf);

  /* Parses a char buffer, ie '26.01.2025 10:56:04'
   * into the DS3231::Time* param tm
   */
  static void ParseDateTime(struct Time* tm, char* datetime);
};
