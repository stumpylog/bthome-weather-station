
#include "tasks.h"

i2c_config_t conf = {
    .mode             = I2C_MODE_MASTER,
    .sda_io_num       = I2C_MASTER_SDA_IO,
    .sda_pullup_en    = GPIO_PULLUP_ENABLE,
    .scl_io_num       = I2C_MASTER_SCL_IO,
    .scl_pullup_en    = GPIO_PULLUP_ENABLE,
    .master.clk_speed = I2C_MASTER_FREQ_HZ,
    .clk_flags        = 0,
};

int i2c_master_port = 0;

void i2c_init(void)
{
    ESP_ERROR_CHECK(i2c_param_config(i2c_master_port, &conf));
    i2c_driver_install(i2c_master_port, I2C_MODE_MASTER, 0, 0, ESP_INTR_FLAG_IRAM);
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (address << 1) | I2C_MASTER_READ, true);
    i2c_master_read(cmd, buf.data, buf.len, i == cnt - 1 ? I2C_MASTER_LAST_NACK : I2C_MASTER_ACK);
    i2c_master_stop(cmd);
    i2c_master_cmd_begin(port_, cmd, 20 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);
}

void task_read_sensors_entry(void* params)
{
    i2c_init();

    for (;;)
    {
    }
}