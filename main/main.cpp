#ifdef __cplusplus
extern "C" {
#endif

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "tasks.h"
#include "blackboard.h"

void app_main(void)
{
    // Read items from NVS
    blackboard_init();

    xTaskCreatePinnedToCore(&task_ble_entry, "ble task", 2048, NULL, 5, &blackboard.system.bleTaskHandle, 0);

    xTaskCreatePinnedToCore(&task_read_sensors_entry, "sensor task", 2048, NULL, 5, &blackboard.system.sensorTaskHandle, 1);
}

#ifdef __cplusplus
}
#endif
