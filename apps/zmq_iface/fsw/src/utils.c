#include "utils.h"

#define ONE_SECOND_IN_MILLISECONDS 1000

int32 SleepOneSecond()
{
    return OS_TaskDelay(ONE_SECOND_IN_MILLISECONDS);
}

