#include "Icarous.h"
#include "trajectory_msgids.h"

#include <json-c/json.h>

typedef enum CommandId {
    TRAJECTORY,
    UNKNOWN
} CommandId ;

static struct json_object * parseCommand(char const * const msgBuffer);
static CommandId getCommandId(struct json_object * commandJSON);
static void processTrajectory(struct json_object * commandJSON);
static char const * json_to_string(struct json_object * obj);

void ZMQ_IFACE_ProcessCommand(char const * const msgBuffer)
{
    OS_printf("Begin processing command: %s\n", msgBuffer);

    struct json_object * commandJSON = parseCommand(msgBuffer);
    if (commandJSON == NULL) {
        return;
    }
    CommandId id = getCommandId(commandJSON);
    switch (id) {
        case TRAJECTORY:
            processTrajectory(commandJSON);
            break;
        default:
            OS_printf("UNKNOWN Command JSON: %s\n", json_to_string(commandJSON));
            break;
    }
    OS_printf("End processing command: %s\n", msgBuffer);
    json_object_put(commandJSON);
}

static struct json_object * parseCommand(char const * const msgBuffer)
{
    struct json_object * commandJSON = json_tokener_parse(msgBuffer);
    if (commandJSON == NULL) {
        OS_printf("ERROR parsing the comand: %s \n",json_tokener_error_desc(json_tokener_get_error(NULL)) );
        return NULL;
    }
    if (json_object_get_type(commandJSON) != json_type_object) {
        OS_printf("ERROR JSON command is not an JSON object: %s\n",json_type_to_name(json_object_get_type(commandJSON)) );
        return NULL;
    }
    return commandJSON;
}


static CommandId getCommandId(struct json_object * commandJSON)
{
    char const * const TRAJECTORY_TYPE_STRING = "ICAROUS::FlightPlan";
    struct json_object * msgTypeJSON = NULL;
    json_bool status = json_object_object_get_ex(commandJSON, "type", &msgTypeJSON);
    if (status == FALSE) {
        OS_printf("JSON \"type\" field does not exist or its null): %s\n", json_to_string(commandJSON));
        return UNKNOWN;
    }
    char const * msgType = json_object_get_string(msgTypeJSON);
    if (msgType == NULL) {
        OS_printf("JSON \"type\" field is not a string: %s\n", json_to_string(commandJSON));
        return UNKNOWN;
    }
    if (strcmp(msgType,TRAJECTORY_TYPE_STRING) == 0) {
        return TRAJECTORY;
    } else {
        return UNKNOWN;
    }
}

static void processTrajectory(struct json_object * commandJSON)
{
        struct json_object * trajectoryJSON = NULL;
        json_bool json_success = json_object_object_get_ex(commandJSON, "data", &trajectoryJSON);
        if (json_success == FALSE) {
            OS_printf("JSON \"data\" field does not exist or its null): %s\n",json_to_string(commandJSON));
            return;
        }
        char const * const data = json_object_get_string(trajectoryJSON);
        CFE_SB_ZeroCopyHandle_t cpyhandle;
        stringdata_t *bigdataptr = (stringdata_t *)CFE_SB_ZeroCopyGetPtr(sizeof(stringdata_t),&cpyhandle);
        CFE_SB_InitMsg(bigdataptr,EUTL1_TRAJECTORY_MID,sizeof(stringdata_t),TRUE);
        strcpy(bigdataptr->buffer,data);
        CFE_SB_TimeStampMsg( (CFE_SB_Msg_t *) bigdataptr );
        int32 status = CFE_SB_ZeroCopySend( (CFE_SB_Msg_t *) bigdataptr,cpyhandle );
        if(status != CFE_SUCCESS) {
        OS_printf("[testbed] Error publishing EUTL trajectory to SB\n");
        } else {
            OS_printf("[testbed] EUTL trajectory successfully published\n");
        }

}

static char const * json_to_string(struct json_object *obj)
{
    return json_object_to_json_string_ext(obj, JSON_C_TO_STRING_PLAIN);
}
