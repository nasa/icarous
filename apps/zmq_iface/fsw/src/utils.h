#include "cfe.h"

#include <json-c/json.h>

int32 SleepOneSecond(void);

#if defined(OS_DEBUG_PRINTF)
#define APP_DEBUG(...) do { OS_printf("%s():%d:",__func__,__LINE__); OS_printf(__VA_ARGS__); } while(0)
#else
#define APP_DEBUG(...) do { ; } while(0)
#endif

struct json_object * json_object_new_string_from_double(double d);

char const * json_get_string_field(struct json_object *, char const * const key);

bool json_get_double_field(double *dest, struct json_object *obj, char const * const key);

char const * json_to_string(struct json_object * obj);
