#include "utils.h"

#define ONE_SECOND_IN_MILLISECONDS 1000

int32 SleepOneSecond()
{
    return OS_TaskDelay(ONE_SECOND_IN_MILLISECONDS);
}

/*
 * json_object_new_string_from_double
 *
 * Helper that uses the 'json-c' serialization for doubles to create a new JSON string object
 * without leaking the intermediate JSON double object.
 */
struct json_object * json_object_new_string_from_double(double d)
{
    struct json_object * doubleObject = json_object_new_double(d);
    char const * const msg = json_object_to_json_string_ext(doubleObject, JSON_C_TO_STRING_PLAIN);
    struct json_object * stringObject = json_object_new_string(msg);
    json_object_put(doubleObject);
    return stringObject;
}

