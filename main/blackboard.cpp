
#include "blackboard.h"

Blackboard blackboard;

// Store the boot count and use it as a packet ID
static RTC_DATA_ATTR uint8_t bootCount{0};

void blackboard_init(void)
{
    blackboard.system.bootCount = bootCount;
    bootCount++;
}