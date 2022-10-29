

#include "blackboard.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "tasks.h"

// app_main must link to C code
#ifdef __cplusplus
extern "C" {
#endif
void app_main(void);

#ifdef __cplusplus
}
#endif

void app_main(void)
{

    ESP_LOGI("main", "Starting up");

    // Read items from NVS
    blackboard_init();

    xTaskCreatePinnedToCore(&task_ble_entry,                  // task
                            BLE_TASK_NAME,                    // task name
                            4096,                             // stack size
                            NULL,                             // parameters
                            5,                                // priority
                            &blackboard.system.bleTaskHandle, // returned task handle
                            0                                 // pinned core
    );

    xTaskCreatePinnedToCore(&task_read_sensors_entry,            // task
                            SENSOR_TASk_NAME,                    // task name
                            2048,                                // stack size
                            NULL,                                // parameters
                            5,                                   // priority
                            &blackboard.system.sensorTaskHandle, // returned task handle
                            1                                    // pinned core
    );
}
