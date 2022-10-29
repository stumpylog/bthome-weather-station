
#include "BME280.h"
#include "blackboard.h"
#include "driver/gpio.h"
#include "driver/i2c.h"
#include "esp_log.h"
#include "i2cBus.h"
#include "tasks.h"

#define SENSOR_TASK_NAME "sensor_task"

static constexpr gpio_num_t I2C_MASTER_SCL_IO {GPIO_NUM_22};
static constexpr gpio_num_t I2C_MASTER_SDA_IO {GPIO_NUM_21};

i2cBus bus(I2C_NUM_0, I2C_MASTER_SCL_IO, I2C_MASTER_SDA_IO);

void i2c_init(void)
{
    ESP_LOGI(SENSOR_TASK_NAME, "Starting i2c init");

    // Configure the hardware I2C port
    bus.init();

    ESP_LOGI(SENSOR_TASK_NAME, "i2c init complete");
}

void read_sensor_data(void)
{
    ESP_LOGI(SENSOR_TASK_NAME, "reading sensor");
    // Create a sensor.  This will initialize and calibrate from the sensor
    sensors::BME280 sensor(bus);

    // Read data.
    sensor.readData();
    blackboard.sensors.temperature = sensor.temperature;
    blackboard.sensors.humidity    = sensor.humidity;
    blackboard.sensors.pressure    = sensor.pressure;

    ESP_LOGI(SENSOR_TASK_NAME, "read temp  %f", blackboard.sensors.temperature);
    ESP_LOGI(SENSOR_TASK_NAME, "read humid %f", blackboard.sensors.humidity);
    ESP_LOGI(SENSOR_TASK_NAME, "read press %f", blackboard.sensors.pressure);

    // For the lowest power, return the sensor to sleep mode
    sensor.setSleepMode();

    ESP_LOGI(SENSOR_TASK_NAME, "reading completed");
}

void task_read_sensors_entry(void* params)
{

    // Configure the i2c parameters
    i2c_init();

    // Populate the sensor data
    read_sensor_data();

    // Notify the BLE task that data is ready
    ESP_LOGI(SENSOR_TASK_NAME, "notify %s", BLE_TASK_NAME);
    xTaskNotifyGive(blackboard.system.bleTaskHandle);

    for (;;)
    { }
}
