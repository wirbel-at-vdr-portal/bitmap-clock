#include "QSPI.h"
#include <Arduino.h>

/*
ESP32-S3 has the following SPI interfaces:
• SPI0 used by ESP32-S3’s GDMA controller and cache to access in-package or off-package flash/PSRAM
• SPI1 used by the CPU to access in-package or off-package flash/PSRAM
• SPI2 is a general purpose SPI controller with access to a DMA channel allocated by the GDMA controller
• SPI3 is a general purpose SPI controller with access to a DMA channel allocated by the GDMA controller

SPI0 and SPI1:
– Supports Single SPI, Dual SPI, Quad SPI, Octal SPI, QPI, and OPI modes
– 8-line SPI mode supports single data rate (SDR) and double data rate (DDR)
– Configurable clock frequency with a maximum of 120 MHz for 8-line SPI SDR/DDR modes
– Data transmission is in bytes

SPI2:
– Supports operation as a master or slave
Connects to a DMA channel allocated by the GDMA controller
– Supports Single SPI, Dual SPI, Quad SPI, Octal SPI, QPI, and OPI modes
– Configurable clock polarity (CPOL) and phase (CPHA)
– Configurable clock frequency
– Data transmission is in bytes
– Configurable read and write data bit order: most-significant bit (MSB) first, or least-significant bit
(LSB) first
– As a master
* Supports 2-line full-duplex communication with clock frequency up to 80 MHz
* Full-duplex 8-line SPI mode supports single data rate (SDR) only
* Supports 1-, 2-, 4-, 8-line half-duplex communication with clock frequency up to 80 MHz
* Half-duplex 8-line SPI mode supports both single data rate (up to 80 MHz) and double data rate
(up to 40 MHz)
* Provides six SPI_CS pins for connection with six independent SPI slaves
* Configurable CS setup time and hold time
– As a slave
* Supports 2-line full-duplex communication with clock frequency up to 60 MHz
* Supports 1-, 2-, 4-line half-duplex communication with clock frequency up to 60 MHz
* Full-duplex and half-duplex 8-line SPI mode supports single data rate (SDR) only
• SPI3:
– Supports operation as a master or slave
– Connects to a DMA channel allocated by the GDMA controller
– Supports Single SPI, Dual SPI, Quad SPI, and QPI modes
– Configurable clock polarity (CPOL) and phase (CPHA)
– Configurable clock frequency
– Data transmission is in bytes
– Configurable read and write data bit order: most-significant bit (MSB) first, or least-significant bit
(LSB) first
– As a master
* Supports 2-line full-duplex communication with clock frequency up to 80 MHz
* Supports 1-, 2-, 4-line half-duplex communication with clock frequency up to 80 MHz
* Provides three SPI_CS pins for connection with three independent SPI slaves
* Configurable CS setup time and hold time
– As a slave
* Supports 2-line full-duplex communication with clock frequency up to 60 MHz
* Supports 1-, 2-, 4-line half-duplex communication with clock frequency up to 60 MHz


*/

//#include "io_pin_remap.h"
//#include "esp32-hal-log.h"

#define SPI_PARAM_LOCK()    do {} while (xSemaphoreTake(paramLock, portMAX_DELAY) != pdPASS)
#define SPI_PARAM_UNLOCK()  xSemaphoreGive(paramLock)


QSPIClass::QSPIClass(uint8_t spi_bus)
    :_spi_num(spi_bus)
    ,_spi(NULL)
    ,_use_hw_ss(false)
    ,_sck(-1)
    ,_miso(-1)
    ,_mosi(-1)
    ,_ss(-1)
    ,_div(0)
    ,_freq(1000000)
    ,_inTransaction(false)
    ,paramLock(NULL)
{
  if (paramLock == NULL) {
     paramLock = xSemaphoreCreateMutex();
     if (paramLock == NULL) {
        return;
        }
    }
}

QSPIClass::~QSPIClass() {
  end();
  if (paramLock != NULL) {
     vSemaphoreDelete(paramLock);
     paramLock = NULL;
     }
}

// pin remapping disabled: use stubs
#undef digitalPinToGPIONumber
#undef gpioNumberToDigitalPin

#define digitalPinToGPIONumber(digitalPin) (digitalPin)
#define gpioNumberToDigitalPin(gpioNumber) (gpioNumber)



void QSPIClass::begin(int8_t sck, int8_t miso, int8_t mosi, int8_t ss) {
  Serial.println(__FUNCTION__);
  if (_spi) {
     Serial.println("_spi != NULL!");
     return;
     }

  if (!_div) _div = spiFrequencyToClockDiv(_freq);

  _spi = spiStartBus(_spi_num, _div, SPI_MODE0, SPI_MSBFIRST);
  if (!_spi) {
     Serial.println("_spi = NULL after spiStartBus !");
     return;
     }
  Serial.println("line 118");

  if (sck == -1 && miso == -1 && mosi == -1 && ss == -1) {
     _sck = (_spi_num == FSPI) ? SCK : -1;
     _miso = (_spi_num == FSPI) ? MISO : -1;
     _mosi = (_spi_num == FSPI) ? MOSI : -1;
     _ss = (_spi_num == FSPI) ? SS : -1;
     }
  else {
     _sck = sck;
     _miso = miso;
     _mosi = mosi;
     _ss = ss;
     }

  spiAttachSCK(_spi, _sck);
  spiAttachMISO(_spi, _miso);
  spiAttachMOSI(_spi, _mosi);
  Serial.println("leaving begin()");
}

void QSPIClass::end() {
  if (!_spi) return;

  spiDetachSCK(_spi, _sck);
  spiDetachMISO(_spi, _miso);
  spiDetachMOSI(_spi, _mosi);
  setHwCs(false);
  spiStopBus(_spi);
  _spi = NULL;
}

void QSPIClass::setHwCs(bool use) {
  if (use && !_use_hw_ss) {
     spiAttachSS(_spi, 0, _ss);
     spiSSEnable(_spi);
     }
  else if (!use && _use_hw_ss) {
     spiSSDisable(_spi);
     spiDetachSS(_spi, _ss);
     }
  _use_hw_ss = use;
}

void QSPIClass::setFrequency(uint32_t freq)
{
    SPI_PARAM_LOCK();
    //check if last freq changed
    uint32_t cdiv = spiGetClockDiv(_spi);
    if(_freq != freq || _div != cdiv) {
        _freq = freq;
        _div = spiFrequencyToClockDiv(_freq);
        spiSetClockDiv(_spi, _div);
    }
    SPI_PARAM_UNLOCK();
}

void QSPIClass::setClockDivider(uint32_t clockDiv)
{
    SPI_PARAM_LOCK();
    _div = clockDiv;
    spiSetClockDiv(_spi, _div);
    SPI_PARAM_UNLOCK();
}

uint32_t QSPIClass::getClockDivider()
{
    return spiGetClockDiv(_spi);
}

void QSPIClass::setDataMode(uint8_t dataMode)
{
    spiSetDataMode(_spi, dataMode);
}

void QSPIClass::setBitOrder(uint8_t bitOrder)
{
    spiSetBitOrder(_spi, bitOrder);
}

void QSPIClass::beginTransaction(SPISettings settings)
{
    SPI_PARAM_LOCK();
    //check if last freq changed
    uint32_t cdiv = spiGetClockDiv(_spi);
    if(_freq != settings._clock || _div != cdiv) {
        _freq = settings._clock;
        _div = spiFrequencyToClockDiv(_freq);
    }
    spiTransaction(_spi, _div, settings._dataMode, settings._bitOrder);
    _inTransaction = true;
}

void QSPIClass::endTransaction()
{
    if(_inTransaction){
        _inTransaction = false;
        spiEndTransaction(_spi);
        SPI_PARAM_UNLOCK(); // <-- Im not sure should it be here or right after spiTransaction()
    }
}

void QSPIClass::write(uint8_t data)
{
    if(_inTransaction){
        return spiWriteByteNL(_spi, data);
    }
    spiWriteByte(_spi, data);
}

uint8_t QSPIClass::transfer(uint8_t data)
{
    if(_inTransaction){
        return spiTransferByteNL(_spi, data);
    }
    return spiTransferByte(_spi, data);
}

void QSPIClass::write16(uint16_t data)
{
    if(_inTransaction){
        return spiWriteShortNL(_spi, data);
    }
    spiWriteWord(_spi, data);
}

uint16_t QSPIClass::transfer16(uint16_t data)
{
    if(_inTransaction){
        return spiTransferShortNL(_spi, data);
    }
    return spiTransferWord(_spi, data);
}

void QSPIClass::write32(uint32_t data)
{
    if(_inTransaction){
        return spiWriteLongNL(_spi, data);
    }
    spiWriteLong(_spi, data);
}

uint32_t QSPIClass::transfer32(uint32_t data)
{
    if(_inTransaction){
        return spiTransferLongNL(_spi, data);
    }
    return spiTransferLong(_spi, data);
}

void QSPIClass::transferBits(uint32_t data, uint32_t * out, uint8_t bits)
{
    if(_inTransaction){
        return spiTransferBitsNL(_spi, data, out, bits);
    }
    spiTransferBits(_spi, data, out, bits);
}

/**
 * @param data uint8_t *
 * @param size uint32_t
 */
void QSPIClass::writeBytes(const uint8_t * data, uint32_t size)
{
    if(_inTransaction){
        return spiWriteNL(_spi, data, size);
    }
    spiSimpleTransaction(_spi);
    spiWriteNL(_spi, data, size);
    spiEndTransaction(_spi);
}

void QSPIClass::transfer(void * data, uint32_t size) 
{ 
   transferBytes((const uint8_t *)data, (uint8_t *)data, size); 
}

/**
 * @param data void *
 * @param size uint32_t
 */
void QSPIClass::writePixels(const void * data, uint32_t size)
{
    if(_inTransaction){
        return spiWritePixelsNL(_spi, data, size);
    }
    spiSimpleTransaction(_spi);
    spiWritePixelsNL(_spi, data, size);
    spiEndTransaction(_spi);
}

/**
 * @param data uint8_t * data buffer. can be NULL for Read Only operation
 * @param out  uint8_t * output buffer. can be NULL for Write Only operation
 * @param size uint32_t
 */
void QSPIClass::transferBytes(const uint8_t * data, uint8_t * out, uint32_t size)
{
    if(_inTransaction){
        return spiTransferBytesNL(_spi, data, out, size);
    }
    spiTransferBytes(_spi, data, out, size);
}

/**
 * @param data uint8_t *
 * @param size uint8_t  max for size is 64Byte
 * @param repeat uint32_t
 */
void QSPIClass::writePattern(const uint8_t * data, uint8_t size, uint32_t repeat)
{
    if(size > 64) {
        return;    //max Hardware FIFO
    }

    uint32_t byte = (size * repeat);
    uint8_t r = (64 / size);
    const uint8_t max_bytes_FIFO = r * size;    // Max number of whole patterns (in bytes) that can fit into the hardware FIFO

    while(byte) {
        if(byte > max_bytes_FIFO) {
            writePattern_(data, size, r);
            byte -= max_bytes_FIFO;
        } else {
            writePattern_(data, size, (byte / size));
            byte = 0;
        }
    }
}

void QSPIClass::writePattern_(const uint8_t * data, uint8_t size, uint8_t repeat)
{
    uint8_t bytes = (size * repeat);
    uint8_t buffer[64];
    uint8_t * bufferPtr = &buffer[0];
    const uint8_t * dataPtr;
    uint8_t dataSize = bytes;
    for(uint8_t i = 0; i < repeat; i++) {
        dataSize = size;
        dataPtr = data;
        while(dataSize--) {
            *bufferPtr = *dataPtr;
            dataPtr++;
            bufferPtr++;
        }
    }

    writeBytes(&buffer[0], bytes);
}


