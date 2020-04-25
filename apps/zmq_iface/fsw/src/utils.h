#include "cfe.h"

int32 SleepOneSecond();

#if defined(OS_DEBUG_PRINTF)
#define APP_DEBUG(...) do { OS_printf("%s():%d:",__func__,__LINE__); OS_printf(__VA_ARGS__); } while(0)
#else
#define APP_DEBUG(...) do { OS_printf("%s():%d:",__func__,__LINE__); OS_printf("This shouldn't be printed"); } while(0)
#endif
