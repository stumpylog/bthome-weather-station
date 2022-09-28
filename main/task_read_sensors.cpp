
#ifdef __cplusplus
extern "C" {
#endif

#include "BME280.h"
#include "blackboard.h"
#include "driver/gpio.h"
#include "driver/i2c.h"
#include "esp_log.h"
#include "tasks.h"

#define I2C_MASTER_SCL_IO  5         /*!<gpio number for i2c master clock  */
#define I2C_MASTER_SDA_IO  6         /*!<gpio number for i2c master data */
#define I2C_MASTER_NUM     I2C_NUM_0 /*!< I2C port number for master dev */
#define I2C_MASTER_FREQ_HZ 100000    /*!< I2C master clock frequency */

i2c_config_t const conf = {
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

i2c_port_t const i2c_master_port = I2C_NUM_0;

void i2c_init(void)
{
    ESP_LOGI("sensor_task", "Starting i2c init");
    // Configure the hardware I2C port
    ESP_ERROR_CHECK(i2c_param_config(i2c_master_port, &conf));
    ESP_ERROR_CHECK(i2c_driver_install(i2c_master_port, I2C_MODE_MASTER, 0, 0, ESP_INTR_FLAG_IRAM));
}

void read_sensor_data(void)
{
    // Create a sensor.  This will initialize and calibrate from the sensor
    sensors::BME280 sensor(i2c_master_port);

    // Read data.  Note that temperature is used in humidity and pressure
    // readings, so it must be done first
    blackboard.sensors.temperature = sensor.readTemperature();
    blackboard.sensors.humidity    = sensor.readHumidity();
    blackboard.sensors.pressure    = sensor.readPressure();

    // For the lowest power, return the sensor to sleep mode
    sensor.setSleepMode();
}

void task_read_sensors_entry(void* params)
{

    // Configure the i2c parameters
    // i2c_init();

    // Populate the sensor data
    // read_sensor_data();
    // TODO Temporary
    vTaskDelay(5000 / portTICK_PERIOD_MS);

    // Notify the BLE task that data is ready
    xTaskNotifyGive(blackboard.system.bleTaskHandle);

    for (;;)
    { }
}

#ifdef __cplusplus
}
#endif