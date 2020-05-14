#include "utils.h"

#define ONE_SECOND_IN_MILLISECONDS 1000

int32 SleepOneSecond(void)
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

char const * json_get_string_field(struct json_object * obj, char const * const key)
{
    if (obj == NULL)
        return NULL;
    struct json_object * field = NULL;
    json_bool success = json_object_object_get_ex(obj, key, &field);
    if (!success)
        return NULL;
    if (json_object_get_type(field) != json_type_string)
        return NULL;
    char const * str = json_object_get_string(field);
    return str;
}

bool json_get_double_field(double *dest, struct json_object *obj, char const * const key)
{
    if (obj == NULL)
        return false;
    struct json_object * field = NULL;
    json_bool success = json_object_object_get_ex(obj, key, &field);
    if (!success)
        return false;
    if (json_object_get_type(field) != json_type_double)
        return false;
    *dest = json_object_get_double(field);
    return true;
}

char const * json_to_string(struct json_object *obj)
{
    return json_object_to_json_string_ext(obj, JSON_C_TO_STRING_PLAIN);
}
