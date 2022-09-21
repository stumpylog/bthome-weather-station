
#ifdef __cplusplus
extern "C" {
#endif

#include "SHT40.h"
#include "driver/gpio.h"
#include "driver/i2c.h"
#include "tasks.h"

#define I2C_MASTER_SCL_IO  5         /*!<gpio number for i2c master clock  */
#define I2C_MASTER_SDA_IO  6         /*!<gpio number for i2c master data */
#define I2C_MASTER_NUM     I2C_NUM_0 /*!< I2C port number for master dev */
#define I2C_MASTER_FREQ_HZ 100000    /*!< I2C master clock frequency */

i2c_config_t conf = {
    .mode          = I2C_MODE_MASTER,
    .sda_io_num    = I2C_MASTER_SDA_IO,
    .scl_io_num    = I2C_MASTER_SCL_IO,
    .sda_pullup_en = GPIO_PULLUP_ENABLE,
    .scl_pullup_en = GPIO_PULLUP_ENABLE,
    .master =
        {
            .clk_speed = I2C_MASTER_FREQ_HZ,
        },
    .clk_flags = 0,
};

int i2c_master_port = 0;

void i2c_init(void)
{
    uint8_t buffer[18];

    ESP_ERROR_CHECK(i2c_param_config(i2c_master_port, &conf));
    i2c_driver_install(i2c_master_port, I2C_MODE_MASTER, 0, 0, ESP_INTR_FLAG_IRAM);
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();

    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (SHT4x_DEFAULT_ADDR << 1) | I2C_MASTER_READ, true);
    i2c_master_read(cmd, &buffer[0], 18, I2C_MASTER_ACK);
    i2c_master_stop(cmd);

    i2c_master_cmd_begin(i2c_master_port, cmd, 20 / portTICK_PERIOD_MS);

    i2c_cmd_link_delete(cmd);



    // float t_ticks = (uint16_t)readbuffer[0] * 256 + (uint16_t)readbuffer[1];
    // float rh_ticks = (uint16_t)readbuffer[3] * 256 + (uint16_t)readbuffer[4];
    // _temperature = -45 + 175 * t_ticks / 65535;
    // _humidity = -6 + 125 * rh_ticks / 65535;
}

void task_read_sensors_entry(void* params)
{
    i2c_init();

    for (;;)
    { }
}

#ifdef __cplusplus
}
#endif