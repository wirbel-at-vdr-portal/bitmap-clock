#pragma once
#include <stdint.h> // uint8_t

class DS3231 {
private:
  uint8_t reg[1+0x12];
  void SetControl(uint8_t flag, bool On);
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
  void ReadRegisters(uint8_t reg, uint8_t count, uint8_t* dest);
  void WriteRegister(uint8_t reg, uint8_t val);
  void GetTime(struct Time* tm);
  void SetTime(struct Time* tm);

  /* When true, the oscillator is started. When set to false,
   * the oscillator is stopped when the DS3231 switches to VBAT
   */
  void EnableOscillatorOnBattery(bool On);

  /* When true, square wave on battery enabled, high impedance
   * otherwise.
   */
  void EnableSquareWaveOnBattery(bool On);

  /* Select square wave frequency:
   *  0 1Hz
   *  1 1024Hz
   *  2 4096Hz
   *  3 8192Hz
   * Any other value: do nothing.
   */
  void SquareWaveRate(uint8_t Rate);

  /* When true, a square wave is output on the INT/SQW pin.
   * Alarm Interrupt otherwise.
   */
  void Pin3SquareWave(bool On);

  /* When true, Alarm 1 / 2 Interrupt is enabled.
   */
  void EnableAlarm1(bool On);
  void EnableAlarm2(bool On);

  /* When true, the 32kHz pin is enabled and outputs a
   * 32.768kHz squarewave signal. When false,
   * the 32kHz pin goes to a high-impedance state.
   */
  void Enable32kHzOutput(bool On);

  /* Positive aging values slow down the oscillator frequency.
   * Negative values increase the oscillator frequency.
   * The change in ppm per LSB is different at different temperatures.
   * At +25°C, one LSB typically provides about 0.1ppm change in frequency.
   */
  void SetAgingOffset(int8_t value);

  /* Measured temperature with a resolution of 0.25°C
   */
  double Temperature(void);

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
