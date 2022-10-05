
#ifdef __cplusplus
extern "C" {
#endif
#include "i2cBus.h"

#include "esp_log.h"

#include <cstring>

#define I2C_MASTER_FREQ_HZ 100000 /*!< I2C master clock frequency */

#define NAME "i2cBus"

i2cBus::i2cBus(i2c_port_t const port, gpio_num_t const scl_io_num, gpio_num_t const sda_io_num)
    : m_port(port), m_scl(scl_io_num), m_sda(sda_io_num)
{ }

i2cBus::~i2cBus(void)
{
    i2c_driver_delete(this->m_port);
}

void i2cBus::init(void)
{
    i2c_config_t const conf = {
        .mode          = I2C_MODE_MASTER,
        .sda_io_num    = this->m_sda,
        .scl_io_num    = this->m_scl,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master =
            {
                .clk_speed = I2C_MASTER_FREQ_HZ,
            },
        .clk_flags = 0,
    };

    ESP_ERROR_CHECK(i2c_param_config(this->m_port, &conf));
    ESP_ERROR_CHECK(i2c_driver_install(this->m_port, I2C_MODE_MASTER, 0, 0, ESP_INTR_FLAG_IRAM));
}

void i2cBus::burstWrite(uint8_t const address, uint8_t const reg, uint8_t const data[], uint8_t const length) const
{
    uint8_t buffer[64];
    buffer[0] = reg;
    memcpy(&buffer[1], data, length);
    ESP_ERROR_CHECK(i2c_master_write_to_device(this->m_port, address, &buffer[0], length + 1, 20 / portTICK_PERIOD_MS));
}

void i2cBus::write8(uint8_t const address, uint8_t const reg, uint8_t value) const
{
    uint8_t buffer[2];
    buffer[0] = static_cast<uint8_t>(reg);
    buffer[1] = value;
    this->burstWrite(address, reg, &buffer[0], 2);
}

void i2cBus::burstRead(uint8_t const address, uint8_t const reg, uint8_t const length, uint8_t dest[]) const
{
    uint8_t buffer[1];
    buffer[0] = reg;
    ESP_ERROR_CHECK(i2c_master_write_read_device(this->m_port, address,
                                                 // Write
                                                 &buffer[0], 1,
                                                 // Read
                                                 &dest[0], length, 10 / portTICK_PERIOD_MS));
}

uint8_t i2cBus::read8(uint8_t const address, uint8_t const reg) const
{
    uint8_t buffer[1];
    this->burstRead(address, reg, 1, &buffer[0]);
    return buffer[0];
}

uint16_t i2cBus::read16(uint8_t const address, uint8_t const reg) const
{
    uint8_t buffer[2];
    this->burstRead(address, reg, 2, &buffer[0]);
    return uint16_t(buffer[0]) << 8 | uint16_t(buffer[1]);
}

int16_t i2cBus::readS16(uint8_t const address, uint8_t const reg) const
{
    return static_cast<int16_t>(read16(address, reg));
}

uint32_t i2cBus::read24(uint8_t const address, uint8_t const reg) const
{
    uint8_t buffer[3];
    buffer[0] = reg;
    ESP_ERROR_CHECK(i2c_master_write_read_device(this->m_port, address,
                                                 // Write
                                                 &buffer[0], 1,
                                                 // Read
                                                 &buffer[0], 3, 20 / portTICK_PERIOD_MS));
    return uint32_t(buffer[0]) << 16 | uint32_t(buffer[1]) << 8 | uint32_t(buffer[2]);
}

uint16_t i2cBus::read16_LE(uint8_t const address, uint8_t const reg) const
{
    uint16_t temp = read16(address, reg);
    return (temp >> 8) | (temp << 8);
}

int16_t i2cBus::readS16_LE(uint8_t const address, uint8_t const reg) const
{
    return (int16_t)read16_LE(address, reg);
}

#ifdef __cplusplus
}
#endif
