#include "QSPI.h"

//Quad
QSPIClass QSPI = QSPIClass(HSPI);///QuadSPI();



void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  while (!Serial);
  delay(100);
  Serial.print("HSPI = "), Serial.println(HSPI);
  Serial.print("FSPI = "), Serial.println(FSPI);
/*
    30, // SPICLK_GPIO30, SCK, GPIO30,     Pin6 SCK
    29, // SPICS0_GPIO29, /CS, GPIO29,     Pin1 /CS
    32, // SPID_GPIO32  , IO0, GPIO32,     Pin5, IO0(SI)
    31, // SPIQ_GPIO31  , IO1, GPIO31,     Pin2, IO1(SO)
    28, // SPIWP_GPIO28 , IO2, GPIO28,     Pin3, IO2(/WP)
    27, // SPIHD_GPIO27 , IO3, GPIO27,     Pin7, IO3(/HOLD)
*/
//QSPI.begin(int8_t sck=-1, int8_t miso=-1, int8_t mosi=-1, int8_t ss=-1);
  QSPI.begin(30, 31, 32, 29);





  Serial.println("entering loop()");
}

void loop() {
  // put your main code here, to run repeatedly:
  SPISettings settings(14000000, MSBFIRST, SPI_MODE0);
  SPI.beginTransaction(settings);
  uint8_t Manufacturer, MemoryType, Capacity;
  SPI.transfer(0x9F);
  Manufacturer = SPI.transfer(0);
  MemoryType = SPI.transfer(0);
  Capacity = SPI.transfer(0);
  SPI.endTransaction();
  Serial.print("0x"); Serial.print(Manufacturer); Serial.print(" ");
  Serial.print("0x"); Serial.print(MemoryType); Serial.print(" ");
  Serial.print("0x"); Serial.print(Capacity); Serial.println(" ");
  delay(1000);




}
