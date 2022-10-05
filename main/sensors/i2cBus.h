#ifndef _I2C_BUS_H_
#define _I2C_BUS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "driver/i2c.h"

#include <cstdint>

class i2cBus
{
  public:
    i2cBus(i2c_port_t const port, gpio_num_t const scl_io_num, gpio_num_t const sda_io_num);
    ~i2cBus(void);

    void init(void);

    void write8(uint8_t const address, uint8_t const reg, uint8_t value) const;
    void burstWrite(uint8_t const address, uint8_t const reg, uint8_t const data[], uint8_t const length) const;

    uint8_t read8(uint8_t const address, uint8_t const reg) const;
    uint16_t read16(uint8_t const address, uint8_t const reg) const;
    uint32_t read24(uint8_t const address, uint8_t const reg) const;
    int16_t readS16(uint8_t const address, uint8_t const reg) const;
    uint16_t read16_LE(uint8_t const address, uint8_t const reg) const; // little endian
    int16_t readS16_LE(uint8_t const address, uint8_t const reg) const; // little endian
    void burstRead(uint8_t const address, uint8_t const reg, uint8_t const length, uint8_t dest[]) const;

  private:
    i2c_port_t m_port;
    gpio_num_t m_scl;
    gpio_num_t m_sda;
};

#ifdef __cplusplus
}
#endif

#endif
