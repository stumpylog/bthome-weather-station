
#ifndef BLACKBOARD_H_
#define BLACKBOARD_H_

typedef struct SensorTaskData
{
    float temperature;
    float humidity;
    float batteryPercent;
} SensorTaskData;

typedef struct Blackboard
{
    SensorTaskData sensors;
} Blackboard;

extern Blackboard blackboard;

#endif