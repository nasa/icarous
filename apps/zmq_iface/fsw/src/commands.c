#include "Icarous.h"
#include "trajectory_msgids.h"
#include "utils.h"

#include <json-c/json.h>

typedef enum CommandId {
    TRAJECTORY,
    OWNSHIP,
    OBJECT,
    UNKNOWN
} CommandId ;

static struct json_object * parseCommand(char const * const msgBuffer);
static CommandId getCommandId(struct json_object * commandJSON);
void processTrajectory(struct json_object * commandJSON);
static void processOwnship(struct json_object * commandJSON);

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
        case OWNSHIP:
            processOwnship(commandJSON);
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
    char const * const OWNSHIP_TYPE_STRING = "ICAROUS::Ownship";
    char const * const OBJECT_TYPE_STRING = "ICAROUS::Object";
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
    } else if (strcmp(msgType,OWNSHIP_TYPE_STRING) == 0) {
        return OWNSHIP;
    } else if (strcmp(msgType,OBJECT_TYPE_STRING) == 0) {
        return OBJECT;
    } else {
        return UNKNOWN;
    }
}

void processTrajectory(struct json_object * commandJSON)
{
    char const * trajectory = json_get_string_field(commandJSON, "data");
    if (trajectory == NULL) {
        OS_printf("JSON \"data\" field of type string does not exist: %s\n",json_to_string(commandJSON));
        return;
    }
    CFE_SB_ZeroCopyHandle_t cpyhandle;
    stringdata_t *bigdataptr = (stringdata_t *)CFE_SB_ZeroCopyGetPtr(sizeof(stringdata_t),&cpyhandle);
    CFE_SB_InitMsg(bigdataptr,EUTL1_TRAJECTORY_MID,sizeof(stringdata_t),TRUE);
    strcpy(bigdataptr->buffer,trajectory);
    CFE_SB_TimeStampMsg( (CFE_SB_Msg_t *) bigdataptr );
    int32 status = CFE_SB_ZeroCopySend( (CFE_SB_Msg_t *) bigdataptr,cpyhandle );
    if(status != CFE_SUCCESS) {
    OS_printf("[testbed] Error publishing EUTL trajectory to SB\n");
    } else {
        OS_printf("[testbed] EUTL trajectory successfully published\n");
    }
}

static void processOwnship(struct json_object * obj)
{
    bool success = false;
    position_t ownship;
    double time = 0;
    success = json_get_double_field(&time, obj, "time");
    if (success == false) {
        OS_printf("[zmq_iface] JSON object double field \"time\" does not exist: %s\n...ignoring Ownship message\n", json_to_string(obj));
        return;
    }
    ownship.time_gps = (int64_t) time;
    ownship.time_boot = ownship.time_gps;
}

