#pragma once
#include <SPI.h>


class QSPIClass {
private:
    int8_t _spi_num;
    spi_t * _spi;
    bool _use_hw_ss;
    int8_t _sck;
    int8_t _miso;
    int8_t _mosi;
    int8_t _ss;
    uint32_t _div;
    uint32_t _freq;
    bool _inTransaction;
#if !CONFIG_DISABLE_HAL_LOCKS
    SemaphoreHandle_t paramLock=NULL;
#endif
    void writePattern_(const uint8_t * data, uint8_t size, uint8_t repeat);
    void spiAttachSCK(spi_t* spi, int8_t sck);

public:
    QSPIClass(uint8_t spi_bus=HSPI);
    ~QSPIClass();
    void begin(int8_t sck=-1, int8_t miso=-1, int8_t mosi=-1, int8_t ss=-1);
    void end();

    void setHwCs(bool use);
    void setBitOrder(uint8_t bitOrder);
    void setDataMode(uint8_t dataMode);
    void setFrequency(uint32_t freq);
    void setClockDivider(uint32_t clockDiv);
    
    uint32_t getClockDivider();

    void beginTransaction(SPISettings settings);
    void endTransaction(void);
    void transfer(void * data, uint32_t size);
    uint8_t transfer(uint8_t data);
    uint16_t transfer16(uint16_t data);
    uint32_t transfer32(uint32_t data);
  
    void transferBytes(const uint8_t * data, uint8_t * out, uint32_t size);
    void transferBits(uint32_t data, uint32_t * out, uint8_t bits);

    void write(uint8_t data);
    void write16(uint16_t data);
    void write32(uint32_t data);
    void writeBytes(const uint8_t * data, uint32_t size);
    void writePixels(const void * data, uint32_t size);//ili9341 compatible
    void writePattern(const uint8_t * data, uint8_t size, uint32_t repeat);

    spi_t * bus(){ return _spi; }
    int8_t pinSS() { return _ss; }
};



