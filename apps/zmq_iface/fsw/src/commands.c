#include "Icarous.h"
#include "trajectory_msgids.h"
#include "utils.h"
#include <time.h>

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
static void processObject(struct json_object * jsonObj);

void ZMQ_IFACE_ProcessCommand(char const * const msgBuffer)
{
    APP_DEBUG("Begin processing command: %s\n", msgBuffer);

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
        case OBJECT:
            processObject(commandJSON);
            break;
        default:
            OS_printf("UNKNOWN Command JSON: %s\n", json_to_string(commandJSON));
            break;
    }
    APP_DEBUG("End processing command: %s\n", msgBuffer);
    json_object_put(commandJSON);
}

static struct json_object * parseCommand(char const * const msgBuffer)
{
    struct json_object * commandJSON = json_tokener_parse(msgBuffer);
    if (commandJSON == NULL) {
        OS_printf("ERROR parsing the comand: %s\n",json_tokener_error_desc(json_tokener_get_error(NULL)) );
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
    OS_printf("[zmq_iface] Error publishing EUTL trajectory to SB\n");
    } else {
        OS_printf("[zmq_iface] EUTL trajectory successfully published\n");
    }
}

static void debugInfoJSONField(struct json_object * obj, char const * const field)
{
    OS_printf("[zmq_iface] JSON object double field \"%s\" does not exist: %s\n...ignoring Ownship message\n", field, json_to_string(obj));
}

static void processOwnship(struct json_object * obj)
{
    bool success = false;
    static position_t ownship;
    CFE_SB_InitMsg(&ownship,ICAROUS_POSITION_MID,sizeof(position_t),TRUE);
    char const * id = NULL;
    char const * const idFieldLabel = "id";
    id = json_get_string_field(obj, idFieldLabel);
    if (id == NULL) {
        debugInfoJSONField(obj, idFieldLabel);
        return;
    }
    memcpy(&ownship.callsign.value, id, MAX_CALLSIGN_LEN);
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME,&ts);
    double time = ts.tv_sec + (double)(ts.tv_nsec)/1E9;
    ownship.time_gps = (int64_t) time;
    ownship.time_boot = ownship.time_gps;
    double latitude = 0;
    char const * const latitudeFieldLabel = "latitude";
    success = json_get_double_field(&latitude, obj, latitudeFieldLabel);
    if (success == false) {
        debugInfoJSONField(obj, latitudeFieldLabel);
        return;
    }
    ownship.latitude = latitude;
    double longitude = 0;
    char const * const longitudeFieldLabel = "longitude";
    success = json_get_double_field(&longitude, obj, longitudeFieldLabel);
    if (success == false) {
        debugInfoJSONField(obj, longitudeFieldLabel);
        return;
    }
    ownship.longitude = longitude;
    double altitude = 0;
    char const * const  altitudeFieldLabel = "altitude";
    success = json_get_double_field(&altitude, obj, altitudeFieldLabel);
    if (success == false) {
        debugInfoJSONField(obj, altitudeFieldLabel);
        return;
    }
    ownship.altitude_abs = altitude;
    ownship.altitude_rel = altitude;
    double velocityNorth = 0;
    char const * const  velocityNorthFieldLabel = "velocityNorth";
    success = json_get_double_field(&velocityNorth, obj, velocityNorthFieldLabel);
    if (success == false) {
        debugInfoJSONField(obj, velocityNorthFieldLabel);
        return;
    }
    ownship.vn = velocityNorth;
    double velocityEast = 0;
    char const * const  velocityEastFieldLabel = "velocityEast";
    success = json_get_double_field(&velocityEast, obj, velocityEastFieldLabel);
    if (success == false) {
        debugInfoJSONField(obj, velocityEastFieldLabel);
        return;
    }
    ownship.ve = velocityEast;
    double velocityDown = 0;
    char const * const  velocityDownFieldLabel = "velocityDown";
    success = json_get_double_field(&velocityDown, obj, velocityDownFieldLabel);
    if (success == false) {
        debugInfoJSONField(obj, velocityDownFieldLabel);
        return;
    }
    ownship.vd = velocityDown;
    ownship.hdg = 0;
    ownship.hdop = 0;
    ownship.vdop = 0;
    ownship.numSats = 0;

    CFE_SB_TimeStampMsg((CFE_SB_Msg_t * ) &ownship);
    int32 status = CFE_SB_SendMsg((CFE_SB_Msg_t * ) &ownship);
    if(status != CFE_SUCCESS) {
        OS_printf("[zmq_iface] Error publishing Ownship message to SB\n");
    } else {
        APP_DEBUG("[zmq_iface] Ownship message successfully published\n");
    }
}

static size_t hashId(char const * const id)
{
    size_t hash = 0;
    for (size_t i = 0; i < strlen(id); i++)
        hash += (size_t) id[i];
    return hash;
}

static void processObject(struct json_object * jsonObj)
{
    bool success = false;
    static object_t object;
    CFE_SB_InitMsg(&object,ICAROUS_TRAFFIC_MID,sizeof(object_t),TRUE);
    object.type = _TRAFFIC_SIM_;
    char const * id = NULL;
    char const * const idFieldLabel = "id";
    id = json_get_string_field(jsonObj, idFieldLabel);
    if (id == NULL) {
        debugInfoJSONField(jsonObj, idFieldLabel);
        return;
    }
    memcpy(object.callsign.value, id, MAX_CALLSIGN_LEN);
    object.index = hashId(id);
    double latitude = 0;
    char const * const latitudeFieldLabel = "latitude";
    success = json_get_double_field(&latitude, jsonObj, latitudeFieldLabel);
    if (success == false) {
        debugInfoJSONField(jsonObj, latitudeFieldLabel);
        return;
    }
    object.latitude = latitude;
    double longitude = 0;
    char const * const longitudeFieldLabel = "longitude";
    success = json_get_double_field(&longitude, jsonObj, longitudeFieldLabel);
    if (success == false) {
        debugInfoJSONField(jsonObj, longitudeFieldLabel);
        return;
    }
    object.longitude = longitude;
    double altitude = 0;
    char const * const  altitudeFieldLabel = "altitude";
    success = json_get_double_field(&altitude, jsonObj, altitudeFieldLabel);
    if (success == false) {
        debugInfoJSONField(jsonObj, altitudeFieldLabel);
        return;
    }
    object.altitude = altitude;
    double velocityNorth = 0;
    char const * const  velocityNorthFieldLabel = "velocityNorth";
    success = json_get_double_field(&velocityNorth, jsonObj, velocityNorthFieldLabel);
    if (success == false) {
        debugInfoJSONField(jsonObj, velocityNorthFieldLabel);
        return;
    }
    object.vn = velocityNorth;
    double velocityEast = 0;
    char const * const  velocityEastFieldLabel = "velocityEast";
    success = json_get_double_field(&velocityEast, jsonObj, velocityEastFieldLabel);
    if (success == false) {
        debugInfoJSONField(jsonObj, velocityEastFieldLabel);
        return;
    }
    object.ve = velocityEast;
    double velocityDown = 0;
    char const * const  velocityDownFieldLabel = "velocityDown";
    success = json_get_double_field(&velocityDown, jsonObj, velocityDownFieldLabel);
    if (success == false) {
        debugInfoJSONField(jsonObj, velocityDownFieldLabel);
        return;
    }
    object.vd = velocityDown;

    CFE_SB_TimeStampMsg((CFE_SB_Msg_t * ) &object);
    int32 status = CFE_SB_SendMsg((CFE_SB_Msg_t * ) &object);
    if(status != CFE_SUCCESS) {
        OS_printf("[zmq_iface] Error publishing Object message to SB\n");
    } else {
        APP_DEBUG("[zmq_iface] Object message successfully published\n");
    }
}

