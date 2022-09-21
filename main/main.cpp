#ifdef __cplusplus
extern "C" {
#endif

#include "blackboard.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "tasks.h"

void app_main(void)
{
    // Read items from NVS
    blackboard_init();

    xTaskCreatePinnedToCore(&task_ble_entry,                  // task
                            "ble task",                       // task name
                            2048,                             // stack size
                            NULL,                             // parameters
                            5,                                // priority
                            &blackboard.system.bleTaskHandle, // returned task handle
                            0                                 // pinned core
    );

    xTaskCreatePinnedToCore(&task_read_sensors_entry,            // task
                            "sensor task",                       // task name
                            2048,                                // stack size
                            NULL,                                // parameters
                            5,                                   // priority
                            &blackboard.system.sensorTaskHandle, // returned task handle
                            1                                    // pinned core
    );
}

#ifdef __cplusplus
}
#endif
