
#ifndef BLACKBOARD_H_
#define BLACKBOARD_H_

typedef struct SensorTaskData
{
    float temperature;
    float humidity;
    float batteryPercent;
    float pressure;
} SensorTaskData;

typedef struct SystemData {
    uint8_t bootCount;
    TaskHandle_t bleTaskHandle;
    TaskHandle_t sensorTaskHandle;
} SystemData;

typedef struct Blackboard
{
    SystemData system;
    SensorTaskData sensors;
} Blackboard;

extern Blackboard blackboard;

void blackboard_init(void);

#endif