#include "coveragetest_common.h"

#include "utils.h"

/*
 * Setup function prior to every test
 */
void ZMQ_IFACE_UT_Setup(void)
{
}

/*
 * Teardown function after every test
 */
void ZMQ_IFACE_UT_TearDown(void)
{
}

void Test_ZMQ_IFACE_json_object_new_string_from_double(void)
{
    double const EXACT_DOUBLE = 6.5;
    struct json_object * obj = json_object_new_string_from_double(EXACT_DOUBLE);
    UtAssert_True(obj != NULL, "it does not return NULL");
    UtAssert_True(json_object_get_type(obj) == json_type_string, "it returns a JSON string object");
    char const * const str = json_object_get_string(obj);
    UtAssert_True(strcmp(str, "6.5") == 0, "it returns a JSON string object");
}

void Test_ZMQ_IFACE_json_get_string_field(void)
{
    UtAssert_True(json_get_string_field(NULL, "key") == NULL, "NULL pointer returns NULL");

    json_object * obj = json_object_new_object(); 
    UtAssert_True(json_get_string_field(obj, "key") == NULL, "JSON object without key returns NULL");
    json_object_put(obj);

    obj = json_object_new_object(); 
    json_object_object_add(obj, "key", json_object_new_double(3));
    UtAssert_True(json_get_string_field(obj, "key") == NULL, "JSON object with non-string value returns NULL");
    json_object_put(obj);

    obj = json_object_new_object(); 
    json_object_object_add(obj, "key", json_object_new_string("value"));
    UtAssert_True(strcmp(json_get_string_field(obj, "key"), "value") == 0, "JSON object with string value returns that value");
    json_object_put(obj);
}

void Test_ZMQ_IFACE_json_get_double_field(void)
{
    double result = -1;
    UtAssert_True(json_get_double_field(&result, NULL, "key") == false, "NULL pointer returns false");
    UtAssert_True(result == -1, "NULL pointer does not change destination");

    json_object * obj = json_object_new_object(); 
    result = -1;
    UtAssert_True(json_get_double_field(&result, obj, "key") == false, "JSON object without key returns false");
    UtAssert_True(result == -1, "JSON object without key returns does not change destination");
    json_object_put(obj);

    obj = json_object_new_object(); 
    json_object_object_add(obj, "key", json_object_new_string(""));
    result = -1;
    UtAssert_True(json_get_double_field(&result, obj, "key") == false, "JSON object with non-double value on key returns false");
    UtAssert_True(result == -1, "JSON object with non-double value on key does not change destination");
    json_object_put(obj);

    obj = json_object_new_object(); 
    json_object_object_add(obj, "key", json_object_new_double(6.1));
    result = -1;
    UtAssert_True(json_get_double_field(&result, obj, "key") == true, "JSON object with double value on key returns true");
    UtAssert_True(result == 6.1, "JSON object with non-double value on key returns that value as a double");
    json_object_put(obj);
}

void Test_ZMQ_IFACE_json_to_string(void)
{
    UtAssert_True(strcmp(json_to_string(NULL),"null") == 0, "NULL pointer returns the \"null\" string");

    json_object * obj = json_object_new_object();
    UtAssert_True(strcmp(json_to_string(obj),"{}") == 0, "empty object returns its literal representation");
    json_object_put(obj);

    obj = json_object_new_object();
    json_object_object_add(obj, "key", json_object_new_string("value"));
    char const * const str = json_to_string(obj);
    UtAssert_True(strcmp(str,"{\"key\":\"value\"}") == 0, "object with a field returns its literal representation");
    json_object_put(obj);

    obj = json_object_new_array();
    UtAssert_True(strcmp(json_to_string(obj),"[]") == 0, "empty array returns its literal representation");
    json_object_put(obj);

    obj = json_object_new_array();
    json_object_array_add(obj, json_object_new_object());
    UtAssert_True(strcmp(json_to_string(obj),"[{}]") == 0, "non-empty array returns its literal representation");
    json_object_put(obj);
}

void UtTest_Setup(void)
{
    ADD_TEST(ZMQ_IFACE_json_get_string_field);
    ADD_TEST(ZMQ_IFACE_json_get_double_field);
    ADD_TEST(ZMQ_IFACE_json_object_new_string_from_double);
    ADD_TEST(ZMQ_IFACE_json_to_string);
}
