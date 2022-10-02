
#ifndef TASKS_H_
#define TASKS_H_

#define BLE_TASK_NAME    "ble_task"
#define SENSOR_TASk_NAME "sensor_task"

void task_ble_entry(void* params);

void task_read_sensors_entry(void* params);

#endif