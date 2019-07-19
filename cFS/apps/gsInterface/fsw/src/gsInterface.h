/**
 * @file gsInterface.h
 * @brief function declarations, definitions of macros, datastructures and global variables for the ground station interface application
 */


#ifndef _gsInterface_h_
#define _gsInterface_h_

#include "cfe.h"
#include "cfe_error.h"
#include "cfe_evs.h"
#include "cfe_sb.h"
#include "cfe_es.h"

#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <network_includes.h>
#include <msgdef/ardupilot_msg.h>
#include <msgids/ardupilot_msgids.h>
#include <msgids/scheduler_msgids.h>

#include <msgdef/traffic_msg.h>
#include <msgids/traffic_msgids.h>
#include <msgdef/tracking_msg.h>
#include <msgids/tracking_msgids.h>
#include <msgdef/trajectory_msg.h>
#include <msgids/trajectory_msgids.h>
#include <msgdef/geofence_msg.h>
#include <msgids/geofence_msgids.h>

#include "mavlink/ardupilotmega/mavlink.h"
#include "port_lib.h"


#define BUFFER_LENGTH 1000  ///< Mavlink message receive buffer size

#define GSINTERFACE_PIPE_NAME "GSINTF_PIPE"
#define GSINTERFACE_PIPE_DEPTH 100

#define SCH_GSINTERFACE_PIPE1_NAME "SCH_GSINTERFACE"

#define PARAM_COUNT 72      ///< Total number of ICAROUS parameters

#define MAX_GEOFENCES 30

/**
 * @defgroup GSINTERFACE
 * @brief An application to interface cFS with an ground station system
 * @ingroup APPLICATIONS
 *
 * @details This application opens two ports, one port connects to the autopilot and the other connects to the ground station.
 * The properties of these ports are defined in the input table. Mavlink packets received from the autopilot are
 * redirected to the ground station and vice versa. Additionally, required information from incoming mavlink messages
 * are extracted and published on to the cFS software bus so that other cFS applications can make use of them.
 * Furthermore, this application also subscribes for commands to the autopilot issued by other cFS applications and
 * converts them to appropriate mavlink messages and sends them to the autopilot. In addition to the autopilot's
 * telemetry that is being piped to the ground station, some Icarous related data (encoded as mavlink messages) is also
 * sent to the ground station. Reading data from the autopilot and groundstation are each on separate threads. The rate
 * at which each thread executes is controlled by the cFS scheduler.
 *
 * @see ARDUPILOT_MESSAGES, ARDUPILOT_MESSAGE_TOPICS, ARDUPILOT_TABLES
 */


/**
 * @struct appdataIntGS_t
 * @brief Structure to hold app data
 */
typedef struct{
    CFE_SB_PipeId_t    INTERFACE_Pipe;      ///< pipe variable
    CFE_SB_PipeId_t    SchInterface_Pipe;  ///< pipe variable
    CFE_SB_MsgPtr_t    INTERFACEMsgPtr;     ///< msg pointer to SB message
    CFE_SB_MsgPtr_t    Sch_MsgPtr;         ///< msg pointer to SB message
    CFE_TBL_Handle_t   INTERFACE_tblHandle; ///< table handle
    port_t gs;                              ///< groundstation port
    int receivingWP;                        ///< waypoint current being received
    int numWaypoints;                       ///< num total waypoints
    int numGeofences;                       ///< Number of geofences
    int recvGeofIndex;                      ///< Index of geofence being received
    int waypointSeq;                        ///< received position waypoint
    int nextWaypointIndex;                  ///< Next waypoint index to goto.
    int hbeatFreqCount;                     ///< Counter for sending hbeat message
    mavlink_mission_item_t ReceivedMissionItems[MAX_WAYPOINTS];
    flightplan_t fpData;
    flightplan_t trajectory;
    argsCmd_t startMission;
    geofence_t gfData[MAX_GEOFENCES];
    uint16_t fenceVertices[MAX_GEOFENCES];
    object_t traffic;
    noArgsCmd_t resetIcarous;
    uint8_t currentApMode;
    uint8_t currentIcarousMode;
    mavlink_param_value_t params[PARAM_COUNT];      ///< array of locally stored parameters
    char param_ids[PARAM_COUNT][16];                ///< array of param_ids for all icarous parameters
    uint32_t wptimer;
    uint32_t rcv_wp_seq;
}appdataIntGS_t;


/**
 * Entry point for app
 */
void gsInterface_AppMain(void);

/**
 * Initialize app properties
 */
void gsInterface_AppInit(void);

/**
 * Clean up variables
 */
void gsInterface_AppCleanUp(void);

/**
 * Get mavlink message from ground station
 */
int GetMAVLinkMsgFromGS(void);

/**
 * Process mavlink message from groundstation and take action
 * @param message mavlink message
 */
void ProcessGSMessage(mavlink_message_t message);

/**
 * Process SB messages from pipes and take action
 */
void gsInterface_ProcessPacket(void);

/**
 * Send heartbeat message
 */
void gsSendHeartbeat();

/**
 * Validate table data
 * @param *TblPtr pointer to table
 */
int32_t gsInterfaceTableValidationFunc(void *TblPtr);

appdataIntGS_t appdataIntGS;

/**
 *  Initialize the array of parameter ids
 */
void gsInterface_InitializeParamIds();

/**
 *  Send all locally stored params over the SB
 */
void gsInterface_PublishParams();

void ConvertMissionItemsToPlan(uint16_t  size, mavlink_mission_item_t items[],flightplan_t* fp);


void wpCallback(uint32_t timer);

#define SendGSMsg(arg) writeMavlinkData(&appdataIntGS.gs,&arg)

#define NextParam appdataIntGS.params[i].param_value;\
i++

#define AddParamId(arg,val) strcpy(appdataIntGS.param_ids[i],arg);\
appdataIntGS.params[i].param_value = val; \
i++

#endif /* _ardupilot_h_ */
