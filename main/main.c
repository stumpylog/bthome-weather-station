#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "tasks.h"

void app_main(void)
{
    xTaskCreatePinnedToCore(&task_ble_entry, "ble task", 2048, NULL, 5, NULL, 0);

    xTaskCreatePinnedToCore(&task_read_sensors_entry, "sensor task", 2048, NULL, 5, NULL, 1);
}
